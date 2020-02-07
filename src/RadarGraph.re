open ReasonReact;
open GraphDataTypes;
open BsGraphUtils;

let calAxisFromDegree = (~isX=true, ~degree:float, ~r:float, ~centerX:float, ~centerY:float) => {
  let rad = degree *. (3.1415926536 /. 180.);
  (isX ? 
    (degree <= 90. || degree >= 270. ? centerX +. Js.Math.abs_float(Js.Math.cos(rad) *. r) : centerX -. Js.Math.abs_float(Js.Math.cos(rad) *. r))
    : 
    (degree > 180. ? centerY +. Js.Math.abs_float(Js.Math.sin(rad) *. r) : centerY -. Js.Math.abs_float(Js.Math.sin(rad) *. r))
  )
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
    ~titleYAxis="Radar Graph",
    ~datas:list(radarGraph),
    ~boundary=defaultBoundaryRadar,
    ~disabledElements=defaultDisabledElements,
    ~colorElements=defaultColors,
    ~dataLength=5,
    ~floatDigit=2,
    ~fontSize=15.0,
    ~tooltip=defaultTooltipTitle
  ) => {

  let baseLineLength = ((dataLength < 3 ? 3 : dataLength));  /* min is 3 */
  let centerPointX = (boundary.positionPoints.maxX -. boundary.positionPoints.minX) /. 2.;
  let centerPointY = (boundary.positionPoints.maxY -. boundary.positionPoints.minY) /. 2.;
  let r = switch (boundary.radius > 0., centerPointX < centerPointY) {
  | (true, _) => boundary.radius
  | (false, true) => centerPointX *. 0.6
  | (false, false) => centerPointY *. 0.6
  };
  let lines = Array.make(baseLineLength, ReasonReact.null)
    |> Array.mapi((idx, _) => {
      let degree = ((360 / baseLineLength) * idx) |> float_of_int;
      (idx === 0 ? 
        [|centerPointX, centerPointY, centerPointX +. r, centerPointY|] 
        : 
        [|centerPointX, centerPointY,
          calAxisFromDegree(~isX=true, ~degree, ~r, ~centerX=centerPointX, ~centerY=centerPointY), 
          calAxisFromDegree(~isX=false, ~degree, ~r, ~centerX=centerPointX, ~centerY=centerPointY)
        |]
      )
    });
  
  let baseLineElements = lines |> Array.to_list |> List.mapi((i, line) => {
    <line 
      x1=floatToPrecision(line[0], 2)
      y1=floatToPrecision(line[1], 2)
      x2=floatToPrecision(line[2], 2)
      y2=floatToPrecision(line[3], 2)
      strokeWidth="2"
      stroke=colorElements.axisLine
    />
  }) |> Array.of_list |> array

  let borderRadarPoints = [|""|];
  lines |> Array.to_list |> List.mapi((i, line) => {
    borderRadarPoints[0] = borderRadarPoints[0] ++ floatToPrecision(line[2], 2) ++ "," ++ floatToPrecision(line[3], 2) ++ " ";
  }) |> ignore; 
  let borderRadar = 
    <polyline 
      points=borderRadarPoints[0]
      fill="none" 
      stroke="black"
    /> ;
  
  let tooltipId = "tooltip-" ++ svgId;
  let circleId = "circle-" ++ svgId;
  
  <div className="svg-category">
    <svg 
      id=svgId
      viewBox=("0 0 " ++ (boundary.graphSize.width |> string_of_int) ++ " " ++ (boundary.graphSize.height |> string_of_int))
      preserveAspectRatio="xMinYMin meet" 
      className="svg-content"
    >
      {
        Array.make(10, ReasonReact.null)
        |> Array.mapi((idx, _) => {
          let i = (idx + 1) |> float_of_int;
          let radius = floatToPrecision(r -. (r /. 10. *. i), 2);
          <>
            (idx === 0 ? 
              <circle 
                cx=floatToPrecision(centerPointX, 2) 
                cy=floatToPrecision(centerPointY, 2) 
                r=floatToPrecision(r, 2) 
                stroke="black" strokeWidth="1" fill="lightgray"
              /> : null)
            <circle 
              cx=floatToPrecision(centerPointX, 2) 
              cy=floatToPrecision(centerPointY, 2) 
              r=radius
              stroke="black" strokeWidth="1" fill=(idx mod 2 === 0 ? "white" : "lightgray")
            />
          </>
        }) |> array
      }
      {baseLineElements}
    </svg>
    <div 
      id=tooltipId
      className="graph-tooltip"
    />
  </div>
};