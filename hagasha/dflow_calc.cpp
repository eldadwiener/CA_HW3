/* 046267 Computer Architecture - Spring 2019 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */
#include <vector>
#include "dflow_calc.h"

#define REGS_NUM 32

using namespace std;

typedef struct regInfo {
	int used_by = -1;
	int falseDependNum = 0;
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
	const unsigned int* m_opsLatency;
	InstInfo* m_progTrace;
	unsigned int m_numOfInsts;
	vector <regInfo> regs_vec;
	vector <cmdInfo> cmds_vec;
	int m_totalRunTime;
private:
	void build_cmds();
    int getDepTotalTime(int dep1, int dep2);
	void update_false_dep(int index);
};

ProgCtx analyzeProg(const unsigned int opsLatency[],  InstInfo progTrace[],  int numOfInsts) {
    ProgCtxClss* ctx = new ProgCtxClss(opsLatency, progTrace, numOfInsts);
    if (ctx == NULL)
    {
        return PROG_CTX_NULL;
    }
    return (ProgCtx)ctx;
}

void freeProgCtx(ProgCtx ctx) {
    ProgCtxClss* _ctx = (ProgCtxClss*)ctx;
    delete _ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    ProgCtxClss* _ctx = (ProgCtxClss*)ctx;
    if(theInst >= _ctx->m_numOfInsts) // idx out of range
        return -1;
    return _ctx->cmds_vec[theInst].deptime;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    ProgCtxClss* _ctx = (ProgCtxClss*)ctx;
    if (theInst >= _ctx->m_numOfInsts) // idx out of range
        return -1;
    *src1DepInst = _ctx->cmds_vec[theInst].dep1;
    *src2DepInst = _ctx->cmds_vec[theInst].dep2;
    return 0;
}

int getRegfalseDeps(ProgCtx ctx, unsigned int reg){
    ProgCtxClss* _ctx = (ProgCtxClss*)ctx;
    if (reg >= REGS_NUM) // idx out of range
        return -1;
	return _ctx->regs_vec[reg].falseDependNum;
}

int getProgDepth(ProgCtx ctx) {
    ProgCtxClss* _ctx = (ProgCtxClss*)ctx;
    return _ctx->m_totalRunTime;
}

// class construction, save all the inputs into the class, and run the analysis command
ProgCtxClss::ProgCtxClss(const unsigned int opsLatency[], InstInfo progTrace[], int numOfInsts) :
	m_opsLatency(opsLatency), m_progTrace(progTrace), m_numOfInsts(numOfInsts),
	regs_vec(REGS_NUM), cmds_vec(numOfInsts), m_totalRunTime(0) {
	build_cmds();
}
/*
	build_cmds - analyze all the commands and keep track of all dependancies and time data
*/
void ProgCtxClss::build_cmds() {
	for (unsigned int i = 0; i < m_numOfInsts; ++i) {
		InstInfo& currCmd = m_progTrace[i];
		// update cmd node data
		int selftime = m_opsLatency[currCmd.opcode];
		cmds_vec[i].dep1 = regs_vec[currCmd.src1Idx].used_by;
		cmds_vec[i].dep2 = regs_vec[currCmd.src2Idx].used_by;
		regs_vec[currCmd.dstIdx].used_by = i;
		cmds_vec[i].deptime = getDepTotalTime(cmds_vec[i].dep1, cmds_vec[i].dep2);
        cmds_vec[i].totaltime = cmds_vec[i].deptime + selftime;
		//check for false dependency
		update_false_dep(i);
		// update total run time (if neede)
		if (m_totalRunTime < cmds_vec[i].totaltime) {
			m_totalRunTime = cmds_vec[i].totaltime;
		}
	}
}

// check if command number "index" has a false dependency on it's source reg
// add 1 to the relevant register's false dep counter if one exists
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
		regs_vec[currCmd.dstIdx].falseDependNum++;
	}
}

// return the max time between the two given dependencies, if both are -1 (no dependency) return 0
int ProgCtxClss::getDepTotalTime(int dep1, int dep2)
{
    int dep1time(0), dep2time(0);
    if (dep1 >= 0)
        dep1time = cmds_vec[dep1].totaltime;
    if (dep2 >= 0)
        dep2time = cmds_vec[dep2].totaltime;
    // return the max dependency time
    return (dep1time > dep2time) ? dep1time : dep2time;
}