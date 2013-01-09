// -*- coding: utf-8 -*-
// Copyright (C) 2012-2013 MUJIN Inc. <rosen.diankov@mujin.co.jp>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <mujincontrollerclient/mujincontrollerclient.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace mujinclient;

int main(int argc, char ** argv)
{
    if( argc < 2 ) {
        printf("need username:password. Example: mujinclienttest myuser:mypass\n");
        return 1;
    }
    ControllerClientPtr controller = CreateControllerClient(argv[1]);

    // get all supported keys
    std::vector<std::string> scenekeys;
    controller->GetScenePrimaryKeys(scenekeys);

    cout << "user has " << scenekeys.size() << " scenes: " << endl;
    for(size_t i = 0; i < scenekeys.size(); ++i) {
        cout << scenekeys[i] << endl;
    }

    SceneResourcePtr scene;
    // if YG_LAYOUT exists, open it, otherwise open the first file
    if( find(scenekeys.begin(),scenekeys.end(), string("YG_LAYOUT")) != scenekeys.end() ) {
        scene.reset(new SceneResource(controller, "YG_LAYOUT"));
    }
    else {
        cout << "opening scene " << scenekeys.at(0) << endl;
        scene.reset(new SceneResource(controller, scenekeys.at(0)));
    }

    // open the first task
    std::vector<std::string> taskkeys;
    scene->GetTaskPrimaryKeys(taskkeys);
    TaskResourcePtr task(new TaskResource(controller, taskkeys.at(0)));
    //task = scene->GetOrCreateTaskFromName("task0");

    cout << "got task " << task->Get("name") << endl;
    cout << "program is " << task->Get("taskgoalxml") << endl;
    // execute task
    //task->Execute();
    // check if task is complete
    //task->GetRunTimeStatus()
    PlanningResultResourcePtr result = task->GetResult();
    std::map<std::string, Transform> transforms;
    if( !!result ) {
        cout << "result for task exists and can be completed in " << result->Get("task_time") << " seconds." << endl;
    }

    // get the first optimization
    std::vector<std::string> optimizationkeys;
    task->GetOptimizationPrimaryKeys(optimizationkeys);
    //OptimizationResourcePtr optimization = task->GetOrCreateOptimizationFromName(optimizationkeys.at(0));
    OptimizationResourcePtr optimization(new OptimizationResource(controller, optimizationkeys.at(0)));
    cout << "found optimization " << optimization->Get("name") << endl;

    std::vector<PlanningResultResourcePtr> results;
    optimization->GetResults(10,results);
    if( results.size() > 0 ) {
        cout << "the top results have times: ";
        for(size_t i = 0; i < results.size(); ++i) {
            cout << results[i]->Get("task_time") << ", ";
        }
        cout << endl;

        PlanningResultResourcePtr bestresult = results.at(0);
        bestresult->GetTransforms(transforms);
        cout << "robot position of best result is: ";
        for(std::map<std::string, Transform>::iterator it = transforms.begin(); it != transforms.end(); ++it) {
            Transform tfirst = it->second;
            // for now only output translation
            cout << it->first << "=(" << tfirst.translation[0] << ", " << tfirst.translation[1] << ", " << tfirst.translation[2] << "), ";
        }
        cout << endl;
        std::string robotprogram = bestresult->Get("robot_programs");
        if( robotprogram.size() > 0 ) {
            cout << endl << "robot program is: " << endl << robotprogram << endl;
        }
        else {
            // output the trajectory in xml format
            cout << endl << "trajectory is: " << endl << bestresult->Get("trajectory") << endl;
        }
    }

    // destroy all mujin controller resources
    ControllerClientDestroy();
}
