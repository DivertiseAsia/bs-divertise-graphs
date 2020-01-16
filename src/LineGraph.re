open ReasonReact;
open GraphDataTypes;
open BsGraphUtils;

let svgWidth = 1000;
let svgHeight = 500;

let minX = 100.;
let maxX = 900.;
let minY = 50.;
let maxY = 400.;

let calculatePoint = (~value:valueLineGraph, ~maxXvalue, ~minXvalue, point, yValueLength) => {
  let axisX = floatToPrecision(
                percentOfData(~data=(value.x -. minXvalue), ~maxValue=(maxXvalue -. minXvalue))
                |> pointFromPercent(~startPoint=minX, ~length=(maxX -. minX), ~isX=true)
                , 2
              );

  let axisY = percentOfData(
                ~data=(value.y < 0. ? 
                  (10. -. Js.Math.abs_float(value.y))
                  : (value.y +. 10.)), 
                ~maxValue=yValueLength
              ) 
              |> pointFromPercent(~startPoint=minY, ~length=(maxY -. minY))
              |> Js.Float.toString;
  point ++ (axisX ++ ",") ++ (axisY ++ "") ++ " ";
};

let findMaxAndMinTime = (~findMax=true, datas:list(lineGraph)) => {
  let allTime = datas |> List.map((data:lineGraph) => {
    let times = data.values |> List.map((value:valueLineGraph) => {
      value.x;
    });
    (findMax ? Js.Math.maxMany_float(times |> Array.of_list) : Js.Math.minMany_float(times |> Array.of_list));
  });
  (findMax ? Js.Math.maxMany_float(allTime |> Array.of_list) : Js.Math.minMany_float(allTime |> Array.of_list));
};

let filterValues = (values, minXvalue, maxXvalue) => values 
      |> List.filter((value:valueLineGraph) => 
        value.x >= minXvalue && value.x <= maxXvalue)

let drawLines = (~maxXvalue, ~minXvalue, ~target, ~yValueLength, datas:list(lineGraph)) => {
  datas |> List.mapi((index, data:lineGraph) => {
    <g 
      key=(data.title ++ "-" ++ (index |> string_of_int))
    >
      {
        let points = [|""|];
        filterValues(data.values, minXvalue, maxXvalue) 
        |> List.mapi((i, value) => {
          points[0] = calculatePoint(~value, ~maxXvalue, ~minXvalue, points[0], yValueLength);
          if (i === List.length(data.values) - 1) {
            points[0] = points[0] ++ lastPoint(data.values, points[0], maxX)
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
    ~maxYvalue=10.,
    ~minYvalue=-10.,
    ~target:string="", 
    ~timeRange=AllTime,
    ~fontColor="black",
    ~showLines=false, 
    ~showPoints=true,
    ~showGuildLine=true,
    ~guildLineColor="gray",
    ~axisLineColor="black",
    ~showBorder=false,
    ~showLineCenter=true,
    ~borderColor="black",
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
  let yValueLength = Js.Math.abs_float(minYvalue -. maxYvalue);

  let tooltipDatas: array(array(array(float))) = 
    datas |> List.map((data:lineGraph) => {
      filterValues(data.values, minXvalue, maxXvalue) |> List.map((value:valueLineGraph) => {
        let point = calculatePoint(~value, ~maxXvalue, ~minXvalue, "", yValueLength) |> Js.String.split(",");
        [|value.y, value.x, point[0] |> float_of_string, point[1] |> float_of_string|]
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

  let minYStr = (minYvalue < 0. ? floatToPrecision(minYvalue, floatDigit) : ((minYvalue === 0. ? "":"+") ++ (floatToPrecision(minYvalue, floatDigit))));
  let middleYvalue = maxYvalue -. (Js.Math.abs_float(minYvalue -. maxYvalue) /. 2.);
  let middleYStr = middleYvalue < 0. ? floatToPrecision(middleYvalue, floatDigit) : ((middleYvalue === 0. ? "":"+") ++ floatToPrecision(middleYvalue, floatDigit));
  let maxYStr = (maxYvalue < 0. ? floatToPrecision(maxYvalue, floatDigit) : ((maxYvalue === 0. ? "":"+") ++ (floatToPrecision(maxYvalue, floatDigit))));
  
  let title:array(string) = datas |> List.map((data:lineGraph) => data.title) |> Array.of_list;
  let viewBox = ("0 0 " ++ (svgWidth |> string_of_int) ++ " " ++ (svgHeight |> string_of_int));
  <div className="svg-line-graph">
    <svg id=svgId viewBox preserveAspectRatio="xMinYMin meet" className="svg-content" onMouseMove onMouseEnter onMouseLeave>
      {
        getXAxisAsTimes(timeRange) 
        |> List.filter(time => time >= minXvalue && time <= maxXvalue) 
        |> List.mapi((i, time) => {
          let x = floatToPrecision(
                percentOfData(~data=(time -. minXvalue), ~maxValue=(maxXvalue -. minXvalue))
                |> pointFromPercent(~startPoint=minX, ~length=(maxX -. minX), ~isX=true)
                , 2
              );
          <line
            key=string_of_int(i)
            x1=x
            y1=floatToPrecision(minY, 2)
            x2=x 
            y2=floatToPrecision(maxY, 2)
            strokeWidth="0.5" 
            stroke="antiquewhite"
          />
        })
        |> Array.of_list |> array
      }
      (showGuildLine ? {drawGuildLineX(~lineAmount=20, ~minX, ~maxX, ~minY, ~maxY, ~strokeColor=guildLineColor)} : null)
      (showBorder ? <polyline points="0,0 0,500 1000,500 1000,0 0,0" fill="none" stroke=borderColor strokeWidth="2" /> : null)
      <line 
        x1=floatToPrecision(minX, 2)
        y1=floatToPrecision(minY, 2)
        x2=floatToPrecision(minX, 2)
        y2=floatToPrecision(maxY, 2)
        strokeWidth="3" 
        stroke=axisLineColor
      />
      (showLineCenter ? <line 
        x1={floatToPrecision(minX, 2)}
        y1=(((maxY -. minY) /. 2. +. minY) |> Js.Float.toString)
        x2={floatToPrecision(maxX, 2)}
        y2=(((maxY -. minY) /. 2. +. minY) |> Js.Float.toString)
        strokeWidth="3" 
        stroke=axisLineColor
      /> : null)
      <text 
        /*x=floatToPrecision(minX -. 85., 2)*/
        x=(floatToPrecision(
            minX -. (fontSize *. 2.) -. 
            ((Js.String.length(maxYStr) |> float_of_int) *. (fontSize /. 3.5))
          , 2))
        y=floatToPrecision(minY +. (fontSize /. 2.5), 2)
        fill=fontColor
        fontSize=(floatToPrecision(fontSize, 2)++"px")
      >
        {string(maxYStr)}
      </text>
      <text 
        x=(floatToPrecision(
          minX -. (fontSize *. 2.) -. 
          ((Js.String.length(middleYStr) |> float_of_int) *. (fontSize /. 3.5))
        , 2))
        y=(((maxY -. minY) /. 2. +. minY +. (fontSize /. 2.5)) |> Js.Float.toString)
        fill=fontColor
        fontSize=(floatToPrecision(fontSize, 2)++"px")
      >
        {string(middleYStr)}
      </text>
      <text 
        x=(floatToPrecision(
          minX -. (fontSize *. 2.) -. 
          ((Js.String.length(minYStr) |> float_of_int) *. (fontSize /. 3.5))
        , 2))
        y=floatToPrecision(maxY +. (fontSize /. 2.5), 2)
        fill=fontColor
        fontSize=(floatToPrecision(fontSize, 2)++"px")
      >
        {string(minYStr)}
      </text>
      {
        <g>
          {drawXvaluesStr(
            ~minX, ~maxX, ~maxY, ~fontColor, ~fontSize, 
            ~dateStart=minXvalue, ~dateEnd=maxXvalue, 
            ~range=(xRange < 2 ? 1 : (xRange - 1)))}
          {showLines ?
            <>
            {datas 
              |> List.filter((t:lineGraph) => t.title !== target)
              |> drawLines(~maxXvalue, ~minXvalue, ~target, ~yValueLength)
            }
            {datas 
              |> List.filter((t:lineGraph) => t.title === target)
              |> drawLines(~maxXvalue, ~minXvalue, ~target, ~yValueLength)
            }
            </> : null
          }
          {showPoints ? pointElements : null}
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
