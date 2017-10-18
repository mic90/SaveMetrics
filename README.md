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
Open the SaveMetrics.pro file in the QtCreator and run the build, all dependencies should be built automatically 
## License
This project is licensed under the GPL License
