// -*- coding: utf-8 -*-
/** \example mujinideal.cpp

    Shows how to execute a task on a specific scene and get the results. If the scene does not exist, will import it first.
 */
#include <mujincontrollerclient/mujincontrollerclient.h>

#include <boost/thread/thread.hpp> // for sleep

#include <iostream>

using namespace mujinclient;

int main(int argc, char ** argv)
{
    if( argc < 2 ) {
        std::cout << "need username:password. Example: mujinclienttest myuser:mypass [url]\n\nurl - [optional] For example https://controller.mujin.co.jp/" << std::endl;
        return 1;
    }
    try {
        ControllerClientPtr controller, controller2;
        // licensekey "username:password"
        //controller = CreateControllerClient(licensekey);
        if( argc >= 5 ) {
            controller = CreateControllerClient(argv[1], argv[2], argv[3], argv[4]);
        }
        if( argc == 4 ) {
            controller = CreateControllerClient(argv[1], argv[2], argv[3]);
        }
        else if( argc == 3 ) {
            controller = CreateControllerClient(argv[1], argv[2]);
        }
        else {
            controller = CreateControllerClient(argv[1]);
        }

        controller->SetDefaultSceneType("wincaps");
        controller->SetDefaultTaskType("itlplanning"); // densowaverc8pcs
        controller->SetCharacterEncoding("Shift_JIS");
        controller->SetLanguage("ja");

        std::vector<JobStatus> statuses;
        while(1) {
            controller->GetRunTimeStatus(statuses);
            if( statuses.size() > 0 ) {
                // something is running
                for(size_t i = 0; i < statuses.size(); ++i) {
                    if( statuses[i].code == JSC_Pending || statuses[i].code == JSC_Active || statuses[i].code == JSC_Preempting || statuses[i].code == JSC_Recalling ) {
                        // ask user to cancel or wait
                        if( user_cancel ) {
                            controller->CancelAllJobs();
                        }
                    }
                }
            }
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        }

        // can execute
        //SceneResourcePtr scene(controller,"WC3");
        //scene.Delete(); // delete tasks and optimizations

        // upload to network drive
        // pac programs should be converted ITL in this call
        controller->SyncUpload("C:\\mywincaps\\my.WPJ");
        SceneResourcePtr scene = controller->RegisterScene("mujin:/my.WPJ");


        TaskResourcePtr task = scene->GetOrCreateTaskFromName("MYPAC1");

        ITLPlanningTaskInfo taskinfo;
        task->GetTaskInfo(taskinfo);
        taskinfo.returntostart = 0;
        taskinfo.speedoptimization = 0; ///< ignore all SPEED/ACCEL commands.
        taskinfo.ignorefigure = 1;
        taskinfo.vrcruns = 1; // 0
        task->SetTaskInfo(taskinfo);

        task->Execute();
        // wait for task to finish

        // query the results until they are complete, should take 100s
        PlanningResultResourcePtr result;
        JobStatus status;
        int iterations = 0, maxiterations = 4000;
        while (1) {
            result = task->GetResult();
            if( !!result ) {
                break;
            }

            task->GetRunTimeStatus(status);
            if( status.code == JSC_Aborted || status.code == JSC_Preempted ) {
                // task failed, tell user
                break;
            }

            if( status.code == JSC_Active ) {
                // task still executing
            }

            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            ++iterations;
        }

        std::string errormessage = result->Get("errormessage");
        if( errormessage.size() > 0 ) {
            // task failed with error message
            return;
        }

        // task succeed
        std::string robotprogram = result->Get("robot_programs");
        std::cout << std::endl << "robot program is: " << std::endl << robotprogram << std::endl;
        std::cout << "final task_time is " << result->Get("task_time") << std::endl;

        OptimizationResourcePtr optimization = task->GetOrCreateOptimizationFromName("MYNAME0","robotplacement");

        RobotPlacementOptimizationInfo optimizationinfo;
        optimizationinfo.name = "casesupply";
        optimizationinfo.frame = "Work1";
        optimizationinfo.unit = "mm";
        optimizationinfo.minrange[0] = -SX; // X
        optimizationinfo.maxrange[0] = SX; // X
        optimizationinfo.stepsize[0] = 100; // X

        optimizationinfo.minrange[3] = -180; // angle
        optimizationinfo.maxrange[3] = 90; // angle
        optimizationinfo.stepsize[3] = 90;
        optimizationinfo.ignorebasecollision = 1;
        optimizationinfo.maxstorecandidates = 0; // infinity
        optimization->SetOptimizationInfo(optimizationinfo);

        optimization->Execute();

        while(1) {
            optimization->GetRunTimeStatus (status);
            if( status == JSC_Succeeded ) {
                break;
            }
            if( status.code == JSC_Aborted || status.code == JSC_Preempted ) {
                // optimization failed, tell user
                break;
            }

            // optimization still computing, wait more
        }

        std::vector< PlanningResultResourcePtr > results;
        optimization->GetResults(0,10,results);
        optimization->GetResults(10,20,results);
        optimization->GetResults(20,30,results);

        for(size_t i = 0; i < results.size(); ++i) {
            results[i]->Get("task_time");
        }


    }
    catch(const MujinException& ex) {
        std::cout << "exception thrown: " << ex.message() << std::endl;
    }
    ControllerClientDestroy();
}
