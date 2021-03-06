# -*- coding: utf-8 -*-
# Copyright (C) 2012-2013 MUJIN Inc. <rosen.diankov@mujin.co.jp>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
include_directories(${MUJINCLIENT_INCLUDE_LOCAL_DIRS})
link_directories(${MUJINCLIENT_LINK_DIRS})

build_sample(mujinexecutetask)
build_sample(mujinexecutetask_fast)
build_sample(mujinexecutetask_robodia)
build_sample(mujinideal_densowave)
build_sample(mujinimportscene)
build_sample(mujinshowresults)
build_sample(mujinimportscene_robodia)
build_sample(mujincalibrationstuff)
if (libzmq_FOUND)
  build_sample(mujinhandeyecalibration)
  build_sample(mujinbinpickingtask)
  build_sample(mujinjog)
  build_sample(mujinupdategeometry)
  build_sample(mujinmovetoollinear)
  build_sample(mujintransform)
  build_sample(mujingetpublishedtaskstate)
  build_sample(mujingrabrelease)
  build_sample(mujinpointcloud)
  build_sample(mujincreateinstobject)
  build_sample(mujinmovetohandposition)
  build_sample(mujinmoveandcancel)
  build_sample(mujinplanexecute)
endif()
build_sample(mujinupdateenvironmentstate)
