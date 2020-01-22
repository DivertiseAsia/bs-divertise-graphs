# bs-divertise-graphs


```Line graph information

    X axis is datetime
    Y axis is float type

    ----- << type lineGraph >> -----
    {
        title: Name of data
        color: Color of line element
        points: list of x axis value and y axis value
    }
    
    Line graph using list of lineGraph type for show datas on graph.
    You need to convert your datas to list of lineGraph type before.

    

    ----- << Props >> -----

    - svgId is id of svg element
    - yIsDatetime is bool for validate Y value type (float or datetime)
    - datas is list of lineGraph type 
    - boundary is min/max of value and point in graph
    - disabledElements is settings disable status of elements in graph
    - colorElements is settings color of elements in graph
    - target, If you set target equal with some title in datas. That line's data will bigest
    - timeRange is start datetime to end dateime from your datas
    - floatDigit is digit position of Y value
    - fontSize is size of text
    - xRange is range of text time for show
    - yLength is range of Y's value for show
    - tooltip is tooltip's title for X axis and y axis
    
```

```Category graph information

    X axis is category's name
    Y axis is float type

    ----- << type categoryGraph >> -----
    {
        title: name of category
        color: color of point,
        point: x axis value and y axis
    }
    
    Category graph using list of categoryGraph type for show datas on graph.
    You need to convert your datas to list of categoryGraph type before.

    

    ----- << Props >> -----

    - svgId is id of svg element
    - titleYAxis is main title for Y axis
    - datas is list of categoryGraph type 
    - boundary is min/max of value and point in graph
    - disabledElements is settings disable status of elements in graph
    - colorElements is settings color of elements in graph
    - yLength is range of Y's value for show
    - timeRange is start datetime to end dateime from your datas
    - floatDigit is digit position of Y value
    - fontSize is size of text
    - randomDistance is distance from center of X's value when had random position //If randomDistance = 0.0, Points will not random position
    - waveXString is toggle
    - tooltip is tooltip's title for X axis and y axis
    
```
