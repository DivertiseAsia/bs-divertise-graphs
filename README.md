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
    - boundary is min/max of value and point in graph
    - disabledElements is settings disable status of elements in graph
    - colorElements is settings color of elements in graph
    - target, If you set target equal with some title in datas. That line's data will bigest.
    - timeRange is start datetime to end dateime from your datas,
    - floatDigit is digit position of Y value
    - fontSize is size of text
    - xRange is range of text time for show,
    - onMouseMove is function for control tooltip
    - onMouseEnter is function for control tooltip
    - onMouseLeave is function for control tooltip
    
```
