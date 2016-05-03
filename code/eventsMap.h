/*
 * eventsMap.h
 *
 *  Created on: 18-Apr-2016
 *      Author: anubhav
 */

#ifndef EVENTSMAP_H_
#define EVENTSMAP_H_

enum PMU_USER_COMMANDS {
	PMU_START_COUNTERS, PMU_STOP_COUNTERS
};

struct pmuStruct {
	int eventToSample;
	int pid;
	int samplingRate;
};

//See <asm/perf_event.h> for these numbers.
static const u32 MSR_ARCH_PERFMON_PMC0 = 0x00C1;
static const u32 MSR_ARCH_PERFMON_EVENTSEL0 = 0x0186;

static const u64 CPU_CYCLES = 0x003c;
static const u64 BRANCH_MISPREDICTIONS = (u64) 0x00C5;

static const u64 ARCH_CLR_OVF_PMC0 = (u64) 0x1 << 0;
static const u64 ARCH_CLR_OVF_BUFFER = (u64) 0x1 << 62;

static const u64 ARCH_PERFMON_EVENTSEL_INT = (u64) 0x1 << 20;
static const u64 ARCH_PERFMON_EVENTSEL_ENABLE = (u64) 0x1 << 22;
static const u64 ARCH_PERFMON_EVENTSEL_USR = (u64) 0x1 << 16;
static const u64 ARCH_PERFMON_EVENTSEL_OS = (u64) 0x1 << 17;

static const u32 MSR_ARCH_PERFMON_GLOBAL_CTRL = 0x038F;
static const u32 MSR_ARCH_PERFMON_GLOBAL_OVF_CTRL = 0x0390;
static const u32 MSR_ARCH_PERFMON_FIXED_CTR_CTRL = 0x038D;


static const short NUMBER_CPUS = 4;

static void clearAllCounters(void) {
	int cpu;
	for_each_online_cpu(cpu)
	{
		wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_PMC0, 0x0);
		wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_GLOBAL_CTRL, 0x0);
		wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_GLOBAL_OVF_CTRL, 0x0);
		wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_EVENTSEL0, 0x0);
	}
}

static void setupCounters(u64 eventToCount, u64 sampleInterval) {
	int cpu;
	for_each_online_cpu(cpu)
	{
		//If using PMC1, need to set bit 1 of this register.
		wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_GLOBAL_OVF_CTRL, ARCH_CLR_OVF_BUFFER);
		wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_EVENTSEL0, (u64) eventToCount | ARCH_PERFMON_EVENTSEL_INT | ARCH_PERFMON_EVENTSEL_ENABLE | ARCH_PERFMON_EVENTSEL_USR | ARCH_PERFMON_EVENTSEL_OS);
		wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_PMC0, sampleInterval);
	}
}

static void enableCounters(int cpu) {
	wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_GLOBAL_CTRL, 0x1);
}

static void disableCounters(int cpu) {
	wrmsrl_safe_on_cpu(cpu, MSR_ARCH_PERFMON_GLOBAL_CTRL, 0x0);
}

static u64 readCounter(u64 reg, u64 sampleInterval) {
	u64 sum = 0;
	int cpu;
	for_each_online_cpu(cpu)
	{
		u64 val;
		rdmsrl_safe_on_cpu(cpu, reg, &val);
		if (val != sampleInterval)
			sum += val;
	printk(KERN_INFO "ValueL %x (%d) : %ld\n",reg,cpu,val);
}
return sum;
}



#endif /* EVENTSMAP_H_ */
