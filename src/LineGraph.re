open ReasonReact;
open GraphDataTypes;
open BsGraphUtils;

let calculatePoint = (~dataPoint:point, ~maxXvalue, ~minXvalue, ~positionPoints, point, yValueLength) => {
  let axisX = floatToPrecision(
                percentOfData(~data=(dataPoint.x -. minXvalue), ~maxValue=(maxXvalue -. minXvalue))
                |> pointFromPercent(~startPoint=positionPoints.minX, ~length=(positionPoints.maxX -. positionPoints.minX), ~isX=true)
                , 2
              );

  let axisY = percentOfData(
                ~data=(dataPoint.y < 0. ? 
                  (10. -. Js.Math.abs_float(dataPoint.y))
                  : (dataPoint.y +. 10.)), 
                ~maxValue=yValueLength
              ) 
              |> pointFromPercent(~startPoint=positionPoints.minY, ~length=(positionPoints.maxY -. positionPoints.minY))
              |> Js.Float.toString;
  point ++ (axisX ++ ",") ++ (axisY ++ "") ++ " ";
};

let findMaxAndMinTime = (~findMax=true, datas:list(lineGraph)) => {
  let allTime = datas |> List.map((data:lineGraph) => {
    let times = data.points |> List.map((point:point) => {
      point.x;
    });
    (findMax ? Js.Math.maxMany_float(times |> Array.of_list) : Js.Math.minMany_float(times |> Array.of_list));
  });
  (findMax ? Js.Math.maxMany_float(allTime |> Array.of_list) : Js.Math.minMany_float(allTime |> Array.of_list));
};

let filterValues = (points, minXvalue, maxXvalue) => points 
      |> List.filter((point:point) => 
      point.x >= minXvalue && point.x <= maxXvalue)

let drawPolyline = (~maxXvalue, ~minXvalue, ~target, ~yValueLength, ~positionPoints, ~drawFill=true, datas:list(lineGraph)) => {
  datas |> List.mapi((index, data:lineGraph) => {
    <g 
      key=(data.title ++ "-line-" ++ (index |> string_of_int))
    >
      {
        let points = [|""|];
        filterValues(data.points, minXvalue, maxXvalue) 
        |> List.mapi((i, point) => {
          points[0] = calculatePoint(~dataPoint=point, ~maxXvalue, ~minXvalue, ~positionPoints, points[0], yValueLength);
          if (i === List.length(data.points) - 1) {
            points[0] = points[0] ++ lastPoint(data.points, points[0], positionPoints.maxX)
          };
        }) |> ignore;
        (Array.length(points) === 1 && points[0] === "" ?
          null :
          (drawFill ? 
            <polyline 
              points=(floatToPrecision(positionPoints.minX, 2) ++ "," ++
                floatToPrecision(positionPoints.maxY, 2) ++ " " ++
                points[0] ++ " " ++ 
                floatToPrecision(positionPoints.maxX, 2) ++ "," ++
                floatToPrecision(positionPoints.maxY, 2) ++ " " ++
                floatToPrecision(positionPoints.minX, 2) ++ "," ++
                floatToPrecision(positionPoints.maxY, 2))
              fill=data.color 
              stroke="none" 
              style=(ReactDOMRe.Style.make(
                ~opacity=(Js.String.toLowerCase(target) === Js.String.toLowerCase(data.title) ? "0.5" : "0.2"), ()))
            /> 
            :
            <polyline 
              points=points[0] 
              fill="none"
              stroke=data.color 
              strokeWidth={
                (Js.String.toLowerCase(target) === Js.String.toLowerCase(data.title) ? "6" : "3")
              }
            /> 
          )
        )
      }
    </g>
  }) |> Array.of_list |> array
};

[@react.component]
let make = (
    ~svgId, 
    ~yIsDatetime,
    ~datas:list(lineGraph)=[], 
    ~boundary=defaultBoundary,
    ~disabledElements=defaultDisabledElements,
    ~colorElements=defaultColors,
    ~target:string="", 
    ~timeRange=AllTime,
    ~floatDigit=2,
    ~fontSize=15.0,
    ~yLength=3,
    ~xRange=4,
    ~tooltip=defaultTooltipTitle
  ) => {

  let (minXvalue, maxXvalue) = switch (timeRange) {
  | Month(number) => (filterByMonth(number), Js.Date.now())
  | Year(number) => (filterByYear(number), Js.Date.now())
  | AllTime => (findMaxAndMinTime(~findMax=false, datas), findMaxAndMinTime(~findMax=true, datas))
  };

  let tooltipId = "tooltip-" ++ svgId;
  let circleId = "circle-" ++ svgId;
  let yValueLength = Js.Math.abs_float(boundary.yValue.min -. boundary.yValue.max);

  let tooltipDatas: array(array(array(float))) = 
    datas |> List.map((data:lineGraph) => {
      filterValues(data.points, minXvalue, maxXvalue) |> List.map((dataPoint:point) => {
        let point = calculatePoint(~dataPoint, ~maxXvalue, ~minXvalue, ~positionPoints=boundary.positionPoints, "", yValueLength) |> Js.String.split(",");
        [|dataPoint.y, dataPoint.x, point[0] |> float_of_string, point[1] |> float_of_string|]
      }) |> Array.of_list;
    }) |> Array.of_list;
  
  let pointElements = tooltipDatas |> Array.to_list |> List.mapi((index, tooltipData) => {
    tooltipData |> Array.to_list |> List.mapi((i, data) => {
      <circle 
        key=("circle-"++(i |> string_of_int)++"-"++svgId)
        cx=(data[2] |> Js.Float.toString) 
        cy=(data[3] |> Js.Float.toString) 
        r="4" 
        fill=(datas |> Array.of_list)[index].color
      />
    }) |> Array.of_list |> array
  }) |> Array.of_list |> array;

  let titles:array(string) = datas |> List.map((data:lineGraph) => data.title) |> Array.of_list;
  let viewBox = ("0 0 " ++ (boundary.graphSize.width |> string_of_int) ++ " " ++ (boundary.graphSize.height |> string_of_int));
  let onMouseEvent = (e) => 
    showTooltip(
      boundary.positionPoints.minX, 
      boundary.positionPoints.maxX, 
      e, titles, tooltipDatas, [|[||]|], 
      tooltipId, svgId, circleId, 
      tooltip.xTitle, tooltip.yTitle, "line-graph");
  <div className="svg-line-graph">
    <svg 
      id=svgId 
      viewBox 
      preserveAspectRatio="xMinYMin meet"
      className="svg-content"
      onMouseMove=onMouseEvent
      onMouseEnter=onMouseEvent
      onMouseLeave=(_ => hideTooltip(tooltipId))
    >
      (!disabledElements.guildLines ? {drawGuildLineX(~lineAmount=20, ~positionPoints=boundary.positionPoints, ~strokeColor=colorElements.guildLines)} : null)
      (!disabledElements.border ? 
        <polyline 
          points=(
            "0,0 0,"++ (boundary.graphSize.height |> string_of_int) ++ 
            " " ++ (boundary.graphSize.width |> string_of_int) ++ "," ++ (boundary.graphSize.height |> string_of_int) ++ 
            " " ++ (boundary.graphSize.width |> string_of_int) ++ ",0 0,0")
          fill="none" 
          stroke=colorElements.border 
          strokeWidth="2" 
        /> 
        : null)
      <line 
        x1=floatToPrecision(boundary.positionPoints.minX, 2)
        y1=floatToPrecision(boundary.positionPoints.minY, 2)
        x2=floatToPrecision(boundary.positionPoints.minX, 2)
        y2=floatToPrecision(boundary.positionPoints.maxY, 2)
        strokeWidth="3" 
        stroke=colorElements.axisLine
      />
      (!disabledElements.middleLine ? 
      <line 
        x1={floatToPrecision(boundary.positionPoints.minX, 2)}
        y1=(((boundary.positionPoints.maxY -. boundary.positionPoints.minY) /. 2. +. boundary.positionPoints.minY) |> Js.Float.toString)
        x2={floatToPrecision(boundary.positionPoints.maxX, 2)}
        y2=(((boundary.positionPoints.maxY -. boundary.positionPoints.minY) /. 2. +. boundary.positionPoints.minY) |> Js.Float.toString)
        strokeWidth="3" 
        stroke=colorElements.axisLine
      /> : null)
      {drawYvalues(~yValueLength, ~yLength, ~boundary, ~floatDigit, ~colorElements, ~fontSize)}
      {
        <g>
          {drawXvaluesStr(
            ~isDatetime=yIsDatetime,
            ~positionPoints=boundary.positionPoints, ~fontColor=colorElements.font, ~fontSize, 
            ~dateStart=minXvalue, ~dateEnd=maxXvalue, 
            ~range=(xRange < 2 ? 1 : (xRange - 1)))}
          {!disabledElements.dataArea ?
            <>
            {datas 
              |> List.filter((t:lineGraph) => t.title !== target)
              |> drawPolyline(~maxXvalue, ~minXvalue, ~target, ~yValueLength, ~positionPoints=boundary.positionPoints)
            }
            {datas 
              |> List.filter((t:lineGraph) => t.title === target)
              |> drawPolyline(~maxXvalue, ~minXvalue, ~target, ~yValueLength, ~positionPoints=boundary.positionPoints)
            }
            </> : null
          }
          {!disabledElements.dataLines ?
            <>
            {datas 
              |> List.filter((t:lineGraph) => t.title !== target)
              |> drawPolyline(~maxXvalue, ~minXvalue, ~target, ~yValueLength, ~positionPoints=boundary.positionPoints, ~drawFill=false)
            }
            {datas 
              |> List.filter((t:lineGraph) => t.title === target)
              |> drawPolyline(~maxXvalue, ~minXvalue, ~target, ~yValueLength, ~positionPoints=boundary.positionPoints, ~drawFill=false)
            }
            </> : null
          }
          {!disabledElements.dataPoints ? pointElements : null}
        </g>
      }
    </svg>
    <div 
      id=tooltipId
      className="graph-tooltip"
    />
  </div>
};
