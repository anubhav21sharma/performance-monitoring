#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <asm/processor.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/nmi.h>
#include <linux/kfifo.h>
#include <linux/string.h>
#include <../../kernel/sched/sched.h>

#include "eventsMap.h"
#include "charDriver.h"

static int totalCount = 0;
static struct task_struct *myTask;
u64 sampleInterval = (u64) 0;

unsigned long *instructionPointerList = NULL;
static unsigned int index = -1;
long int ind = 0;

extern void (*contextSwitchHook)(struct task_struct*, struct task_struct*, int);
extern void (*taskExitHook)(struct task_struct*);

long int totalCalls = 0;
extern struct file_operations fops;

extern long device_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
	if (!instructionPointerList) {
		return 0;
	}
	if (ind >= totalCalls - 1) {
		ind = 0;
		printk(KERN_WARNING "End reading! \n");
		return 0;
	}
	int bunch = 2000;
	int nChars = 7;
	char *output = vmalloc(nChars * bunch);
	int i, j = 0;
	for (i = 0; i < bunch && ind < totalCalls - 1; i++) {
		sprintf(output + i * nChars, "%x\n", instructionPointerList[ind++]);
	}
	//printk(KERN_WARNING "Copying %d characters. Ind = %d. \n",i*nChars,ind);
	copy_to_user(buffer, output, i * nChars);

	return i * nChars;
}

static int __kprobes overflowHandler(unsigned int cmd, struct pt_regs *regs) {
	int cpu = smp_processor_id();
	disableCounters(cpu);
	instructionPointerList[++index] = task_pt_regs(myTask)->ip;

	totalCalls++;

	wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_PMC0, (u64) sampleInterval);
	enableCounters(cpu);
}

int registerOverflowHandler(u64 sampleInterval) {
	apic_write(APIC_LVTPC, APIC_DM_NMI);
	return register_nmi_handler(NMI_LOCAL, overflowHandler, 0, "PMU_INTERRUPT");
}

void contextSwitchHookFunction(struct task_struct *prev, struct task_struct *next, int cpu) {
	if (prev->pid == myTask->pid) {
		disableCounters(cpu);
	}

	if (next->pid == myTask->pid) {
		enableCounters(cpu);
	}
}

void taskExitHookFunction(struct task_struct *task) {
	if (task->pid == myTask->pid) {
		unregister_nmi_handler(NMI_LOCAL, "PMU_INTERRUPT");

		contextSwitchHook = NULL;
		taskExitHook = NULL;
		printk(KERN_INFO"Task Exited! \n");
	}
}

int requestEventCounter(struct pmuStruct pst) {
	myTask = pid_task(find_vpid(pst.pid), PIDTYPE_PID);
	if (myTask == NULL) {
		printk(KERN_ALERT "PID is invalid.\n");
		return -2;
	}

	totalCalls = 0;
	totalCount = 0;
	index = -1;
	ind = 0;

	u64 eventToCount = pst.eventToSample;
	sampleInterval = (u64) pst.samplingRate;

	clearAllCounters();
	setupCounters(eventToCount, sampleInterval);
	readCounter(MSR_ARCH_PERFMON_PMC0, sampleInterval);

	if (instructionPointerList) {
		vfree(instructionPointerList);
	}
	instructionPointerList = vmalloc(180000000 * sizeof(unsigned long));
	memset(instructionPointerList, 0, 180000000);
	if (instructionPointerList == 0) {
		printk(KERN_ALERT "vmalloc failed! \n");
		return -3;
	}

	if (registerOverflowHandler(sampleInterval)) {
		printk(KERN_ALERT "Failed to register overflow handler.\n");
		return -4;
	}

	contextSwitchHook = contextSwitchHookFunction;
	taskExitHook = taskExitHookFunction;
	return 0;
}

static long pmu_ioctl(struct file *f, unsigned int cmd, unsigned long arg) {
	printk(KERN_INFO "ioctl\n");

	if (cmd == PMU_START_COUNTERS) {

		struct pmuStruct pst = *((struct pmuStruct *) arg);
		printk(KERN_INFO "Starting PMU Counter on Event %x. PID = %d, IPID = %d\n", pst.eventToSample, current->pid, pst.pid);
		return requestEventCounter(pst);

	} else if (cmd == PMU_STOP_COUNTERS) {

		int cpu = smp_processor_id();
		disableCounters(cpu);
		printk(KERN_INFO "Stopping PMU Counter\n");

		if (sampleInterval == 0) {
			unsigned long long reg = MSR_ARCH_PERFMON_PMC0;
			u64 counterValue = readCounter(reg, sampleInterval);
			printk(KERN_INFO "Counters stopped! %ld \n", counterValue);
			*((unsigned long long *) arg) = counterValue;
		} else {
			u64 counterValue = -sampleInterval * totalCalls;
			printk(KERN_INFO "Counters stopped! %ld \n", counterValue);
			*((unsigned long long *) arg) = counterValue;
		}

	}

	return 0;
}

struct file_operations pmufops = { .unlocked_ioctl = pmu_ioctl, .read = device_read };

static int __init perfModuleInit(void) {
	printk(KERN_WARNING "PMU Module : Registered.\n");
	int major = register_chrdev(261, "pmuDriver", &pmufops);
	if (major < 0) {
		printk(KERN_INFO"Unable to register driver! \n");
		return -1;
	}

	printk(KERN_INFO "pmuDriver registered = 261. \n");

	return 0;
}

void __exit perfModuleExit(void) {
	contextSwitchHook = NULL;
	taskExitHook = NULL;
	if (instructionPointerList)
		vfree(instructionPointerList);
	unregister_chrdev(261, "pmuDriver");
	printk(KERN_INFO "PMU Module : Exited.\n");
}

module_init(perfModuleInit);
module_exit(perfModuleExit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PMU Module");
