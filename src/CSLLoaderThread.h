#include "ptypes.h"
#include "pasync.h"

#include "XPMPMultiplayerCSL.h"

const int MSG_LOADJOB_OBJ = 1;
const int MSG_LOADJOB_TEX = 2;
const int MSG_LOADJOB_TEX_LIT = 3;

const int NOT_LOADED_TEX_LIT=-6;
const int NOT_LOADED_TEX=-5;
const int LOADING_TEX=-4;
const int NOT_LOADED_OBJ=-3;
const int LOADING_OBJ=-2;

class loadjobthread: public pt::thread
{
protected:
    int id;
    pt::jobqueue* jq;
    virtual void execute();
public:
    loadjobthread(int iid, pt::jobqueue* ijq)
        : thread(true), id(iid), jq(ijq)  {}
    ~loadjobthread()  { }
};



class CSLLoaderType
{
protected:
    pt::jobqueue *jq;
public:
    CSLLoaderType()  { jq=new pt::jobqueue(); }
    ~CSLLoaderType()  {}
	void startthreads();
	void load(CSLPlane_t* toload);
	void loadTex(CSLPlane_t* toload);
	void loadTexLIT(CSLPlane_t* toload);
};
