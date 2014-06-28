#include "CSLLoaderThread.h"
#include "ptypes.h"
#include "pasync.h"

#include "XPMPMultiplayerCSL.h"
#include "XPMPMultiplayerObj.h"

USING_PTYPES

const int maxthreads = 1;



class loadjob: public message
{
public:
    CSLPlane_t* toload;
    loadjob(const int msg, CSLPlane_t* itoload)
        : message(msg)  { toload = itoload; }
    ~loadjob()  { delete toload; }
};


void loadjobthread::execute()
{
   bool quit = false;
   while (!quit)
    {
        // get the next message from the queue
        message* msg = jq->getmessage();

        try
        {
            switch (msg->id)
            {
            case MSG_LOADJOB_OBJ:
                {
                    CSLPlane_t* toload = ((loadjob*)msg)->toload;
					toload->obj_idx = OBJ_LoadModel(toload->file_path.c_str());
                }
                break;

            case MSG_LOADJOB_TEX:
                {
                    CSLPlane_t* toload = ((loadjob*)msg)->toload;
					toload->texID = OBJ_LoadTexture(toload->tex_path.c_str(), false);
                }
                break;

            case MSG_LOADJOB_TEX_LIT:
                {
                    CSLPlane_t* toload = ((loadjob*)msg)->toload;
					toload->texLitID = OBJ_LoadTexture(toload->texLit_path.c_str(), false);
                }
                break;

            case MSG_QUIT:
                // MSG_QUIT is not used in our example
                quit = true;
                break;
            }
        }
        catch(...)
        {
            // the message object must be freed!
            delete msg;
            throw;
        }
        delete msg;
    }
}



void CSLLoaderType::startthreads()
{
    // create the thread pool
    static tobjlist<loadjobthread> threads(true);
    int i;
    for(i = 0; i < maxthreads; i++)
    {
        static loadjobthread* j = new loadjobthread(i + 1, jq);
        j->start();
        threads.add(j);
    }
}

void CSLLoaderType::load(CSLPlane_t* toload)
{
	jq->post(new loadjob(MSG_LOADJOB_OBJ, toload));
}

void CSLLoaderType::loadTex(CSLPlane_t* toload)
{
	jq->post(new loadjob(MSG_LOADJOB_TEX, toload));
}

void CSLLoaderType::loadTexLIT(CSLPlane_t* toload)
{
	jq->post(new loadjob(MSG_LOADJOB_TEX_LIT, toload));
}
