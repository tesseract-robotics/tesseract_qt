^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tesseract_qt
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Forthcoming
-----------
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
