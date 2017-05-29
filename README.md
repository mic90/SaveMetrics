# SaveMetrics
SaveMetrics is application for personal savings calculation. 
It has embedded lua interpreter so one should have full freedom in writing proper scripts or even complex financial models.

![alt text](https://github.com/mic90/SaveMetrics/blob/master/screenshots/screen_0.png)
## Usage
+ Create new project
+ Set years span for calculation
+ Create some lua files
+ Lua interpreter will look for setup() and process(month, year) functions, so You should declare them in one file
+ setup() function will be run only once, the process() function will be run for every month for the year span selected
+ All user declared global variables will be rendered on chart
+ Run and see the results, by default scripts are processed on file save
## Building
To build the application You should have fully working installation of Qt 5.8.x.
Before building project build required dependencies:
+ Build luajit from included sources -> run /external/luajit-2.0.4/build_windows.bat
+ Build qscintilla from included sources -> /external/QScintilla_gpl-2.10/Qt4Qt5/qscintilla.pro
+ Build qslog from included sources -> /external/qslog/qslog.pro
## LuaRocks integration
To use LuaRocks for installing additional lua libraries, run setup_windows.bat from project root directory.

After that You can use installed libraries by simply including them in scripts: require('socket')
## License
This project is licensed under the GPL License
