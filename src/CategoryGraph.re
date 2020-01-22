open ReasonReact;
open GraphDataTypes;
open BsGraphUtils;

let randomX = (x, randomDistance) => (((x |> float_of_string) +. (Js.Math.random() *. (Js.Math.random_int(-1, 2) |> float_of_int) *. randomDistance)) |> Js.Float.toString);

let calculatePoint = (~y:float, ~title:string, ~allCategory, ~yValueLength, ~positionPoints, point) => {
  let specIndex = Belt.Array.getIndexBy((allCategory |> Array.of_list), (spec => spec === title));
  let axisX = switch specIndex {
  | Some(index) => floatToPrecision(
      percentOfData(~data=(index + 1 |> float_of_int), ~maxValue=(List.length(allCategory) |> float_of_int))
      |> pointFromPercent(~startPoint=positionPoints.minX, ~length=(positionPoints.maxX -. positionPoints.minX), ~isX=true)
      , 2);
  | None => "-50"
  };

  let axisY = percentOfData(
                ~data=(y < 0. ? 
                  (10. -. Js.Math.abs_float(y))
                  : (y +. 10.)), 
                ~maxValue=yValueLength
              ) 
              |> pointFromPercent(~startPoint=positionPoints.minY, ~length=(positionPoints.maxY -. positionPoints.minY))
              |> Js.Float.toString;
  point ++ (axisX ++ ",") ++ (axisY ++ "") ++ " ";
};

let allCategory = (~datas:list(categoryGraph)) => {
  datas
  |> List.sort_uniq((data1:categoryGraph, data2:categoryGraph) 
      => compare(data1.title, data2.title)) 
  |> List.map((data:categoryGraph) => {
    data.title
  });
};

[@react.component]
let make = (
    ~svgId,
    ~titleYAxis="Category Graph",
    ~datas:list(categoryGraph),
    ~boundary=defaultBoundary,
    ~disabledElements=defaultDisabledElements,
    ~colorElements=defaultColors,
    ~yLength=3,
    ~floatDigit=2,
    ~fontSize=15.0,
    ~randomDistance=50.,
    ~waveXString=false,
    ~tooltip=defaultTooltipTitle
  ) => {

  let allCategory = allCategory(~datas);
  let tooltipId = "tooltip-" ++ svgId;
  let circleId = "circle-" ++ svgId;

  let yValueLength = Js.Math.abs_float(boundary.yValue.min -. boundary.yValue.max);

  let tooltipDatas: array(array(string)) = 
    datas |> List.map((data:categoryGraph) => {
      let point = 
        calculatePoint(~y=data.point.y, ~title=data.title, ~allCategory, ~positionPoints=boundary.positionPoints, ~yValueLength, "") 
        |> Js.String.split(",");
      [|data.point.y |> Js.Float.toString, 
        data.title, 
        randomX(point[0], randomDistance), 
        point[1],
        data.color
      |]
    }) |> Array.of_list;
  
  let pointElements = tooltipDatas |> Array.to_list |> List.mapi((i, data) => {
      <circle 
        key=("circle-"++(i |> string_of_int)++"-"++svgId)
        cx=data[2]
        cy=data[3]
        r=(data[4] !== "gray" ? "6" : "3")
        fill=data[4]
        stroke=(data[4] !== "gray" ? "white" : "none")
      />
  }) |> Array.of_list |> array;

  let titles:array(string) = datas |> List.map((data:categoryGraph) => data.title) |> Array.of_list;

  let onMouseEvent = (e) => 
      showTooltip(
        boundary.positionPoints.minX, 
        boundary.positionPoints.maxX, 
        e, titles, [|[||]|], tooltipDatas, 
        tooltipId, svgId, circleId, 
        tooltip.xTitle, tooltip.yTitle, "category-graph");
  <div className="svg-category">
    <svg 
      id=svgId
      viewBox=("0 0 " ++ (boundary.graphSize.width |> string_of_int) ++ " " ++ (boundary.graphSize.height |> string_of_int))
      preserveAspectRatio="xMinYMin meet" 
      className="svg-content"
      onMouseMove=onMouseEvent
      onMouseEnter=onMouseEvent
      onMouseLeave=(_ => hideTooltip(tooltipId))
    >
      {!disabledElements.guildLines ? drawGuildLineX(~lineAmount=20, ~positionPoints=boundary.positionPoints, ~strokeColor="black") : null}
      <line 
        x1=floatToPrecision(boundary.positionPoints.minX, 2)
        y1=floatToPrecision(boundary.positionPoints.minY, 2)
        x2=floatToPrecision(boundary.positionPoints.minX, 2)
        y2=floatToPrecision(boundary.positionPoints.maxY, 2)
        strokeWidth="2"
        stroke=colorElements.axisLine
      />
      <line
        x1={floatToPrecision(boundary.positionPoints.minX, 2)}
        y1={floatToPrecision(boundary.positionPoints.maxY, 2)}
        x2={floatToPrecision(boundary.positionPoints.maxX, 2)}
        y2={floatToPrecision(boundary.positionPoints.maxY, 2)}
        strokeWidth="2"
        stroke=colorElements.axisLine
      />
      <text 
        x=floatToPrecision(boundary.positionPoints.minX -. 20., 2)
        y=(((boundary.positionPoints.maxY -. boundary.positionPoints.minY) /. 2. +. boundary.positionPoints.minY +. 7.) |> Js.Float.toString)
        fill=colorElements.font
        fontSize=(floatToPrecision(fontSize, 2)++"px")
        transform="translate(-210 340) scale(1) rotate(-90)"
      >
        {string(titleYAxis)}
      </text>
      {drawYvalues(~yValueLength, ~yLength, ~boundary, ~floatDigit, ~colorElements, ~fontSize)}
      {
        <g>
          {
            allCategory |> List.mapi((i, spec) => {
              let x = percentOfData(~data=(i + 1 |> float_of_int), ~maxValue=(List.length(allCategory) |> float_of_int))
                      |> pointFromPercent(~startPoint=boundary.positionPoints.minX, ~length=(boundary.positionPoints.maxX -. boundary.positionPoints.minX), ~isX=true);
              <g key=("g-"++(i |> string_of_int)++"-"++svgId)>
                <text 
                  x=(floatToPrecision(x -. ((Js.String.length(spec) |> float_of_int) *. (fontSize /. 3.5)), 2))
                  y=(floatToPrecision(
                    boundary.positionPoints.maxY +. 
                    (((i mod 2) === 0 && waveXString) ? 60. : 30.)
                    , 2))
                  fill=colorElements.font
                  fontSize=(floatToPrecision(fontSize, 2)++"px")
                >
                  {string(spec)}
                </text>
                <line 
                  x1={floatToPrecision(x, 2)}
                  y1={floatToPrecision(boundary.positionPoints.minY, 2)}
                  x2={floatToPrecision(x, 2)}
                  y2={floatToPrecision(boundary.positionPoints.maxY, 2)}
                  strokeWidth="0.5" 
                  stroke=colorElements.axisLine
                />
              </g>
            }) |> Array.of_list |> array
          }
          {pointElements}
        </g>
      }
    </svg>
    <div 
      id=tooltipId
      className="graph-tooltip"
    />
  </div>
};