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
let drawLines = (~maxXvalue, ~minXvalue, ~target, ~yValueLength, ~positionPoints, datas:list(lineGraph)) => {
  datas |> List.mapi((index, data:lineGraph) => {
    <g 
      key=(data.title ++ "-" ++ (index |> string_of_int))
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
          <polyline 
            points=points[0] 
            fill="none"
            stroke=data.color 
            strokeWidth={
              (Js.String.toLowerCase(target) === Js.String.toLowerCase(data.title) ? "6" : "3")
            }
          />
        )
      }
    </g>
  }) |> Array.of_list |> array
};

[@react.component]
let make = (
    ~svgId, 
    ~datas:list(lineGraph)=[], 
    ~boundary=defaultBoundary,
    ~disabledElements=defaultDisabledElements,
    ~colorElements=defaultColors,
    ~target:string="", 
    ~timeRange=AllTime,
    ~floatDigit=2,
    ~fontSize=15.0,
    ~xRange=9,
    ~onMouseMove=(_ => ()),
    ~onMouseEnter=(_ => ()),
    ~onMouseLeave=(_ => ())
  ) => {

  let (minXvalue, maxXvalue) = switch (timeRange) {
  | OneMonth => (filterByMonth(1), Js.Date.now())
  | TwoMonths => (filterByMonth(2), Js.Date.now())
  | ThreeMonths => (filterByMonth(3), Js.Date.now())
  | FourMonths => (filterByMonth(4), Js.Date.now())
  | FiveMonths => (filterByMonth(5), Js.Date.now())
  | SixMonths => (filterByMonth(6), Js.Date.now())
  | SevenMonths => (filterByMonth(7), Js.Date.now())
  | EightMonths => (filterByMonth(8), Js.Date.now())
  | NineMonths => (filterByMonth(9), Js.Date.now())
  | TenMonths => (filterByMonth(10), Js.Date.now())
  | ElevenMonths => (filterByMonth(11), Js.Date.now())
  | OneYear => (filterByYear(1), Js.Date.now())
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
    tooltipData |> Array.to_list |> List.map((data) => {
      <circle 
        cx=(data[2] |> Js.Float.toString) 
        cy=(data[3] |> Js.Float.toString) 
        r="4" 
        fill=(datas |> Array.of_list)[index].color
      />
    }) |> Array.of_list |> array
  }) |> Array.of_list |> array;

  let minYStr = (boundary.yValue.min < 0. ? 
                  floatToPrecision(boundary.yValue.min, floatDigit) : 
                  ((boundary.yValue.min === 0. ? "":"+") ++ (floatToPrecision(boundary.yValue.min, floatDigit))));
  let middleYvalue = boundary.yValue.max -. (Js.Math.abs_float(boundary.yValue.min -. boundary.yValue.max) /. 2.);
  let middleYStr = middleYvalue < 0. ? 
                    floatToPrecision(middleYvalue, floatDigit) : 
                    ((middleYvalue === 0. ? "":"+") ++ floatToPrecision(middleYvalue, floatDigit));
  let maxYStr = (boundary.yValue.max < 0. ? 
                  floatToPrecision(boundary.yValue.max, floatDigit) : 
                  ((boundary.yValue.max === 0. ? "":"+") ++ (floatToPrecision(boundary.yValue.max, floatDigit))));
  
  let title:array(string) = datas |> List.map((data:lineGraph) => data.title) |> Array.of_list;
  let viewBox = ("0 0 " ++ (boundary.graphSize.width |> string_of_int) ++ " " ++ (boundary.graphSize.height |> string_of_int));
  <div className="svg-line-graph">
    <svg id=svgId viewBox preserveAspectRatio="xMinYMin meet" className="svg-content" onMouseMove onMouseEnter onMouseLeave>
      {
        getXAxisAsTimes(timeRange) 
        |> List.filter(time => time >= minXvalue && time <= maxXvalue) 
        |> List.mapi((i, time) => {
          let x = floatToPrecision(
                percentOfData(~data=(time -. minXvalue), ~maxValue=(maxXvalue -. minXvalue))
                |> pointFromPercent(
                  ~startPoint=boundary.positionPoints.minX, 
                  ~length=(boundary.positionPoints.maxX -. boundary.positionPoints.minX), 
                  ~isX=true), 2);
          <line
            key=string_of_int(i)
            x1=x
            y1=floatToPrecision(boundary.positionPoints.minY, 2)
            x2=x 
            y2=floatToPrecision(boundary.positionPoints.maxY, 2)
            strokeWidth="0.5" 
            stroke="antiquewhite"
          />
        })
        |> Array.of_list |> array
      }
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
      (!disabledElements.middleLine ? <line 
        x1={floatToPrecision(boundary.positionPoints.minX, 2)}
        y1=(((boundary.positionPoints.maxY -. boundary.positionPoints.minY) /. 2. +. boundary.positionPoints.minY) |> Js.Float.toString)
        x2={floatToPrecision(boundary.positionPoints.maxX, 2)}
        y2=(((boundary.positionPoints.maxY -. boundary.positionPoints.minY) /. 2. +. boundary.positionPoints.minY) |> Js.Float.toString)
        strokeWidth="3" 
        stroke=colorElements.axisLine
      /> : null)
      <text 
        x=(floatToPrecision(
          boundary.positionPoints.minX -. (fontSize *. 2.) -. 
            ((Js.String.length(maxYStr) |> float_of_int) *. (fontSize /. 3.5))
          , 2))
        y=floatToPrecision(boundary.positionPoints.minY +. (fontSize /. 2.5), 2)
        fill=colorElements.font
        fontSize=(floatToPrecision(fontSize, 2)++"px")
      >
        {string(maxYStr)}
      </text>
      <text 
        x=(floatToPrecision(
          boundary.positionPoints.minX -. (fontSize *. 2.) -. 
          ((Js.String.length(middleYStr) |> float_of_int) *. (fontSize /. 3.5))
        , 2))
        y=(((boundary.positionPoints.maxY -. boundary.positionPoints.minY) /. 2. +. boundary.positionPoints.minY +. (fontSize /. 2.5)) |> Js.Float.toString)
        fill=colorElements.font
        fontSize=(floatToPrecision(fontSize, 2)++"px")
      >
        {string(middleYStr)}
      </text>
      <text 
        x=(floatToPrecision(
          boundary.positionPoints.minX -. (fontSize *. 2.) -. 
          ((Js.String.length(minYStr) |> float_of_int) *. (fontSize /. 3.5))
        , 2))
        y=floatToPrecision(boundary.positionPoints.maxY +. (fontSize /. 2.5), 2)
        fill=colorElements.font
        fontSize=(floatToPrecision(fontSize, 2)++"px")
      >
        {string(minYStr)}
      </text>
      {
        <g>
          {drawXvaluesStr(
            ~positionPoints=boundary.positionPoints, ~fontColor=colorElements.font, ~fontSize, 
            ~dateStart=minXvalue, ~dateEnd=maxXvalue, 
            ~range=(xRange < 2 ? 1 : (xRange - 1)))}
          {!disabledElements.dataLines ?
            <>
            {datas 
              |> List.filter((t:lineGraph) => t.title !== target)
              |> drawLines(~maxXvalue, ~minXvalue, ~target, ~yValueLength, ~positionPoints=boundary.positionPoints)
            }
            {datas 
              |> List.filter((t:lineGraph) => t.title === target)
              |> drawLines(~maxXvalue, ~minXvalue, ~target, ~yValueLength, ~positionPoints=boundary.positionPoints)
            }
            </> : null
          }
          {!disabledElements.dataPoints ? pointElements : null}
        </g>
      }
      <circle id=circleId cx="-50" cy="-50" r="10" fill="white" />
    </svg>
    <div 
      id=tooltipId
      className="graph-tooltip"
    />
  </div>
};
