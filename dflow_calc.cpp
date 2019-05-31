/* 046267 Computer Architecture - Spring 2019 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */
#include <vector>
#include "dflow_calc.h"

#define REGS_NUM 32

using namespace std;

typedef struct regInfo {
	int used_by = -1;
	int falseDepnedNum = 0;
} regInfo;

typedef struct cmdInfo {
	int deptime;
	int totaltime;
	int dep1 = -1;
	int dep2 = -1;
} cmdInfo;

class ProgCtxClss {
public:
	ProgCtxClss(const unsigned int opsLatency[], InstInfo progTrace[], int numOfInsts);
private:
	void build_cmds();
	const unsigned int* m_opsLatency;
	void update_false_dep(int index);
	InstInfo* m_progTrace;
	int m_numOfInsts;
	vector <regInfo> regs_vec;
	vector <cmdInfo> cmds_vec;
	int m_totalRunTime;
};

ProgCtx analyzeProg(const unsigned int opsLatency[],  InstInfo progTrace[],  int numOfInsts) {
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getRegfalseDeps(ProgCtx ctx, unsigned int reg){
	return -1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


ProgCtxClss::ProgCtxClss(const unsigned int opsLatency[], InstInfo progTrace[], int numOfInsts) :
	m_opsLatency(opsLatency), m_progTrace(progTrace), m_numOfInsts(numOfInsts),
	regs_vec(REGS_NUM), cmds_vec(numOfInsts), m_totalRunTime(0) {
	build_cmds();
}
/*
	build_cmds build .. TODO
*/
void ProgCtxClss::build_cmds() {
	for (int i = 0; i < m_numOfInsts; ++i) {
		InstInfo& currCmd = m_progTrace[i];
		// update cmd node data
		cmds_vec[i].selftime = m_opsLatency[currCmd.opcode];
		cmds_vec[i].dep1 = regs_vec[currCmd.src1Idx].used_by;
		cmds_vec[i].dep2 = regs_vec[currCmd.src2Idx].used_by;
		regs_vec[currCmd.dstIdx].used_by = i;
		cmds_vec[i].totaltime = getDepTime(i);
		//check for false dependency
		update_false_dep(i);
		// update total run time (if neede)
		if (m_totalRunTime < cmds_vec[i].totaltime) {
			m_totalRunTime = cmds_vec[i].totaltime;
		}
	}
}

void ProgCtxClss::update_false_dep(int index) {
	// check if it's the first cmd
	if (index == 0) {
		return;
	}
	InstInfo& currCmd = m_progTrace[index];
	InstInfo& prevCmd = m_progTrace[index-1];
	//check for WAR or WAW
	if ((currCmd.dstIdx == prevCmd.dstIdx)//WAW
		|| (currCmd.dstIdx == prevCmd.src1Idx || currCmd.dstIdx == prevCmd.src2Idx))//WAR
	{
		regs_vec[currCmd.dstIdx].falseDepnedNum++;
	}
}