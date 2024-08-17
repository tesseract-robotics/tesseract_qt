^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tesseract_qt
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.24.1 (2024-08-16)
-------------------
* Update task composer node info standard item
* Contributors: Levi Armstrong

0.24.0 (2024-08-15)
-------------------
* Add any poly support for contact result map types
* Add long and unsigned long support
* Contributors: Levi Armstrong

0.23.1 (2024-08-02)
-------------------
* Add poly factories
* Contributors: Levi Armstrong

0.23.0 (2024-07-24)
-------------------
* Update ubuntu.yml
* Add CI for Ubuntu Noble (`#116 <https://github.com/tesseract-robotics/tesseract_qt/issues/116>`_)
* Do not export plugin libraries (`#118 <https://github.com/tesseract-robotics/tesseract_qt/issues/118>`_)
* Update docker.yml
* Update because TaskComposerProblem was removed
* Fixes for building on Ubuntu Noble (`#114 <https://github.com/tesseract-robotics/tesseract_qt/issues/114>`_)
* Contributors: Levi Armstrong, Roelof Oomen

0.22.1 (2024-06-12)
-------------------
* Update the workflow to use tesseract_planning 0.22
* Contributors: Tyler Marr

0.22.0 (2024-06-10)
-------------------
* Upgrade ros_industrial_cmake_boilerplate to version 0.6.0
* Update to changes in task composer node info structure
* Update command language instructions standard item to include uuid and parent uuid
* Add support for jerk limits
* Add missing dependency
* Update CI to correct branch
* Update readme
* Leverage forward declarations
* Fix main branch to point to master (`#104 <https://github.com/tesseract-robotics/tesseract_qt/issues/104>`_)
* Fix clang format for newer versions (`#102 <https://github.com/tesseract-robotics/tesseract_qt/issues/102>`_)
* Rename trajectoryDurationStart to trajectoryDurationBegin (to match ...End)
* Fix trajectory player for trajectories that do not start from time=0, e.g. when in a trajectory set
* Fix crash on row change (`#101 <https://github.com/tesseract-robotics/tesseract_qt/issues/101>`_)
* Enable custom naming in trajectory preview panel
* Add support for polygon mesh
* Remove deprecated AnalyzeTemporaryDtors
  See https://github.com/llvm/llvm-project/issues/62020
* Contributors: Doug Smith, Levi Armstrong, Roelof, Roelof Oomen, Tyler Marr

0.21.3 (2023-12-22)
-------------------
* Fix ogre rendering crash when link only has collision geometry
* Fix branch name in docker CI
* Contributors: Levi Armstrong

0.21.2 (2023-11-21)
-------------------
* Use tesseract_planning docker image (`#94 <https://github.com/tesseract-robotics/tesseract_qt/issues/94>`_)
* Added docker files
* Contributors: Levi Armstrong, Michael Ripperger

0.21.1 (2023-11-17)
-------------------
* Add colcon.pkg
* Remove use of Industrial CI from clang-format CI
* Remove use of Industrial CI (`#90 <https://github.com/tesseract-robotics/tesseract_qt/issues/90>`_)
* Contributors: Levi Armstrong

0.21.0 (2023-11-10)
-------------------
* Update dependencies.rosinstall
* Fixe an include (required for newer GCC on Ubuntu 22.04)
* Update to support changes in tesseract_planning
* Member results was removed from TaskComposerNodeInfo
* Contributors: Levi Armstrong, Roelof Oomen

0.20.2 (2023-10-29)
-------------------
* Fix sendEvent calls
* Contributors: Levi Armstrong

0.20.1 (2023-10-13)
-------------------
* Fix usage of uninitialized members
* Contributors: Roelof Oomen

0.20.0 (2023-09-29)
-------------------
* Update to Tesseract version 0.20.0 (`#79 <https://github.com/tesseract-robotics/tesseract_qt/issues/79>`_)
* Update to use TaskComposerContext (`#74 <https://github.com/tesseract-robotics/tesseract_qt/issues/74>`_)
* Contributors: Levi Armstrong

0.19.0 (2023-09-06)
-------------------
* Add RICB to rosinstall files
* Update to latest tesseract and tesseract_planning
* Fix build issue with configure_package
* Made studio and rendering module builds optional
* Moved plot docker to studio module
* Create gazebo_utils target
* Contributors: Levi Armstrong, Michael Ripperger, Roelof Oomen

0.18.3 (2023-07-10)
-------------------
* Fix manipulation widget when use_parent_component_info is false
* Contributors: Levi Armstrong

0.18.2 (2023-07-10)
-------------------
* Separate Studio App and MainWindow (`#66 <https://github.com/tesseract-robotics/tesseract_qt/issues/66>`_)
* Remove invalid header from yaml model (`#65 <https://github.com/tesseract-robotics/tesseract_qt/issues/65>`_)
* Add YAML Model (`#64 <https://github.com/tesseract-robotics/tesseract_qt/issues/64>`_)
* Add command line argument support to studio
* Contributors: Levi Armstrong

0.18.1 (2023-07-03)
-------------------
* Fix task composer problem model
* Contributors: Levi Armstrong

0.18.0 (2023-07-01)
-------------------
* Update depend to 0.18.X
* Add conversion for polygon mesh to gz::common::SubMesh and leverage for collision mesh
* Fix load tool path
* Fix ignition rendering of capsule
* Changes to support task composer restructure
* Fixes build errors on Jammy/Humble
* Contributors: Levi Armstrong, Roelof Oomen

0.17.0 (2023-06-06)
-------------------
* Update depends
* Support loading config without having to close
* Cleanup tesseract_qt common
* Cleanup singleton classes
* Minor fixes
* Fix contact result arrow visualization
* Fix manipulation widget reload
* Add studio plugin support (`#46 <https://github.com/tesseract-robotics/tesseract_qt/issues/46>`_)
* Contributors: Levi Armstrong

0.16.4 (2023-05-04)
-------------------
* Upgrade gazebo version (`#44 <https://github.com/tesseract-robotics/tesseract_qt/issues/44>`_)
* Contributors: Levi Armstrong

0.16.3 (2023-05-03)
-------------------
* Update dependencies.rosinstall
* Update ign converter to scale mesh
* Contributors: Levi Armstrong

0.16.2 (2023-04-28)
-------------------
* Update depends
* Add waypoint name to waypoint standard items
* Move toolbars to the dock widgets
* Add studio application
* Set the environment state instead of using scene state
* Add working frame to tool path
* Add ability to load and save tool path
* Contributors: Levi Armstrong

0.16.1 (2023-04-11)
-------------------
* Update depends
* Contributors: Levi Armstrong

0.16.0 (2023-04-10)
-------------------
* Update rosinstall depends
* Add trajectory link command support
* Update for ContactResultMapClass
* Fixed environment segfaults (`#32 <https://github.com/tesseract-robotics/tesseract_qt/issues/32>`_)
* Add support for TOTG node info class
* Contributors: Levi Armstrong, afrixs

0.15.2 (2023-03-22)
-------------------
* Update rosinstall depends
* Include fix for Jammy/Humble
* Add more details to node info standard items
* Fix cartesian waypoint standard item seed text
* Update dependencies.rosinstall
* Contributors: Levi Armstrong, Roelof Oomen

0.15.1 (2023-03-14)
-------------------
* Update dependencies.rosinstall
* Remove process planning request standard item
* Contributors: Levi Armstrong

0.15.0 (2023-03-03)
-------------------
* Update rosinstall depends
* Switch to leveraging event filters (`#22 <https://github.com/tesseract-robotics/tesseract_qt/issues/22>`_)
* Removed start instructions as they are no longer used
* Fix tool path render visibility
* Sync render widget with monitor refresh rate
* Add tool path icon and finish event filters
* Add tool path render manager
* Add tool path selection model
* Add ToolPathToolBar
* Tool Path Changes
* Add SceneEvent class to remove duplication in events
* Fix use of removeRow by passing in index parent so correct row is removed
* Update interactive view control to latest fixing large jumps
* Add tool path widgets
* Add render widget to the srdf editor application
* Add rendering widget leveraging ignition
* Update dependencies.rosinstall
* Contributors: Levi Armstrong, Tyler Marr

0.14.0 (2022-10-23)
-------------------
* Fix QToolbox theme
* Add SRDF Editor Widget
* Add task composer data storage standard item
* Add image viewer widget and converting dot to image utility
* Replace tesseract_process_managers with tesseract_task_composer
* Some include fixes for Ubuntu 22.04
* Fix planning problem model when tree item is selected that is not a planning problem
* Update rosinstall tags
* Contributors: Levi Armstrong, Roelof Oomen

0.13.0 (2022-08-25)
-------------------
* Add find package and link library to tesseract_command_language
* Update to use new Poly types in tesseract_planning
* Fix use of appendJointState
* Update to latest version
* Contributors: Levi Armstrong, Tyler Marr

0.2.0 (2022-07-07)
------------------
* Update depends tags
* Support multi-level namespaces and process planning problem model
* Fix license header in files
* Contributors: Levi Armstrong

0.1.2 (2022-06-08)
------------------
* Make widgets layouts not constrained
* Improve manipulation support
* Contributors: Levi Armstrong

0.1.1 (2022-06-03)
------------------
* Fix minor issues (`#2 <https://github.com/tesseract-robotics/tesseract_qt/issues/2>`_)
* Contributors: Levi Armstrong

0.1.0 (2022-05-17)
------------------
* Update CI yaml files to reference main branch
* CMake lint repository
* fix readme
* Initial commit
* Initial commit
* Contributors: Levi Armstrong
