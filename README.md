# bs-divertise-graphs


```Line graph information

    X axis is datetime
    Y axis is float type

    ----- << type lineGraph >> -----
    {
        title: Name of data
        color: Color of line element
        values: list of x axis value and y axis value
    }
    
    Line graph using list of lineGraph type for show datas on graph.
    You need to convert your datas to list of lineGraph type before.

    

    ----- << Props >> -----

    - svgId is id of svg element
    - datas is list of lineGraph type 
    - maxYvalue is maximum value of Y axis (Can be minus number)
    - minYvalue is minimum value of Y axis (Can be minus number)
    - target, If you set target equal with some title in datas. That line's data will bigest.
    - timeRange is start datetime to end dateime from your datas,
    - fontColor is color of text    
    - showLines is toggle lines from graph
    - showPoints is toggle points from graph
    - showGuildLine is toggle sub line of Y axis
    - guildLineColor is color of guild line
    - axisLineColor is color of main line axis
    - showBorder is toggle of graph's border
    - showLineCenter is toggle of main center line of Y axis
    - borderColor is color of graph's border
    - floatDigit is digit position of Y value
    - fontSize is size of text
    - xRange is range of text time for show,
    - onMouseMove is function for control tooltip
    - onMouseEnter is function for control tooltip
    - onMouseLeave is function for control tooltip
    
```
