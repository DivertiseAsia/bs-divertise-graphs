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
    ~floatDigit=2,
    ~fontSize=20.0,
    ~tooltip=defaultTooltipTitle
  ) => {
  
  let allTypeArray = [||];
  let maxValueArray = [|0.|];
  datas |> List.map((data:radarGraph) => {
    data.radarDetails 
    |> List.sort_uniq((radarDetail1, radarDetail2) => compare(radarDetail1.name, radarDetail2.name))
    |> List.map(radarDetail => {
      maxValueArray[0] = (maxValueArray[0] < radarDetail.value ? radarDetail.value : maxValueArray[0]);
      Js.Array.push(radarDetail.name, allTypeArray);
    }) |> ignore;
  }) |> ignore;

  let maxValue = maxValueArray[0];
  let allType = allTypeArray |> Array.to_list |> List.sort_uniq((name1, name2) => compare(name1, name2));

  let centerPointX = (boundary.positionPoints.maxX -. boundary.positionPoints.minX) /. 2.;
  let centerPointY = (boundary.positionPoints.maxY -. boundary.positionPoints.minY) /. 2.;
  let r = switch (boundary.radius > 0., centerPointX < centerPointY) {
  | (true, _) => boundary.radius
  | (false, true) => centerPointX *. 0.6
  | (false, false) => centerPointY *. 0.6
  };
  let lines = allType |> List.mapi((idx, typeStr) => {
      let degree = ((360 / List.length(allType)) * idx) |> float_of_int;
      (idx === 0 ? 
        [|
          centerPointX |> Js.Float.toString, 
          centerPointY |> Js.Float.toString, 
          (centerPointX +. r) |> Js.Float.toString, 
          centerPointY |> Js.Float.toString, 
          typeStr, 
          degree |> Js.Float.toString|] 
        : 
        [|
          centerPointX |> Js.Float.toString, 
          centerPointY |> Js.Float.toString,
          calAxisFromDegree(~isX=true, ~degree, ~r, ~centerX=centerPointX, ~centerY=centerPointY) |> Js.Float.toString, 
          calAxisFromDegree(~isX=false, ~degree, ~r, ~centerX=centerPointX, ~centerY=centerPointY) |> Js.Float.toString, 
          typeStr,
          degree |> Js.Float.toString
        |]
      )
    });
  
  let baseLineElements = lines |> List.mapi((i, line) => {
    let degreeFloat = line[5] |> float_of_string;
    let txtLength = String.length(line[4]) |> float_of_int;
    let fontCalculate = (txtLength /. 2. *. fontSize);
    let x = calAxisFromDegree(~isX=true, ~degree=degreeFloat, ~r=r, ~centerX=centerPointX, ~centerY=centerPointY);
    let y = calAxisFromDegree(~isX=false, ~degree=degreeFloat, ~r=r, ~centerX=centerPointX, ~centerY=centerPointY);
    let key = ("g-base-line-" ++ (i |> string_of_int));
    <g key>
      <line 
        x1={floatToPrecision((line[0] |> float_of_string), 2)}
        y1={floatToPrecision((line[1] |> float_of_string), 2)}
        x2={floatToPrecision((line[2] |> float_of_string), 2)}
        y2={floatToPrecision((line[3] |> float_of_string), 2)}
        strokeWidth="2"
        stroke=colorElements.axisLine
      />
      <text 
        x={floatToPrecision(x +. 
            switch (degreeFloat |> int_of_float) {
            | d when d > 270 || d < 90 => fontCalculate *. (0.3)
            | d when d < 270 && d > 90 => -.(fontCalculate *. 1.1)
            | _ => fontCalculate *. -.0.5
            }
          , 2)}
        y={floatToPrecision(y +. 
          switch (degreeFloat |> int_of_float) {
          | d when d < 180 && d > 0 => fontCalculate *. -.(0.6)
          | d when d > 180 && d < 360 => -.(fontCalculate *. -.0.6)
          | _ => 0.
          }
        , 2)}
        fill=colorElements.font
        fontSize=(floatToPrecision(fontSize, 2) ++ "px")
      >
        {string(line[4])}
      </text>
    </g>
  }) |> Array.of_list |> array

  let dataElements = datas |> List.map((data:radarGraph) => {
    
    let centerPoint = (centerPointX |> Js.Float.toString) ++ "," ++ (centerPointY |> Js.Float.toString);
    let polyPoints = [||];
    let circlePoints = [||];
    let firstDegree = [|-1.|];
    let secondDegree = [|-1.|];
    let lastDegree = [|0.|];
    data.radarDetails 
      |> List.sort_uniq((radarDetail1, radarDetail2) => compare(radarDetail1.name, radarDetail2.name))
      |> List.map((radarDetail) => {
        lines |> List.map((line) => {
          switch (radarDetail.name === line[4]) {
          | true => {
              let radForPoint = (r *. percentOfData(~data=radarDetail.value, ~maxValue) /. 100.);
              let x = 
                floatToPrecision(
                  calAxisFromDegree(
                      ~isX=true, 
                      ~degree=(line[5] |> float_of_string), 
                      ~r=radForPoint, 
                      ~centerX=centerPointX, ~centerY=centerPointY
                    )
                  , 2);
              let y = 
                floatToPrecision(
                  calAxisFromDegree(
                      ~isX=false, 
                      ~degree=(line[5] |> float_of_string), 
                      ~r=radForPoint,  
                      ~centerX=centerPointX, ~centerY=centerPointY
                    )
                  , 2);
              if (firstDegree[0] === -1.) {
                firstDegree[0] = (line[5] |> float_of_string);
              } else {
                if (secondDegree[0] === -1.) {
                  secondDegree[0] = (line[5] |> float_of_string);
                };
              };
              lastDegree[0] = (line[5] |> float_of_string);
              Js.Array.push([|x,y|], circlePoints) |> ignore;
              Js.Array.push(x ++ "," ++ y, polyPoints) |> ignore;
              ()
            }
          | false => ()
          }
        }) |> ignore;
      }) |> ignore;
    let allPoint = [|""|];
    polyPoints |> Array.to_list |> List.mapi((idxPoint, point:string) => {
      if (idxPoint === 0 && (lastDegree[0] -. firstDegree[0]) < 180.) {
        (allPoint[0] = allPoint[0] ++ centerPoint ++ " ")
      } else if (idxPoint === 1 && (360. -. secondDegree[0]) < 180.) {
        (allPoint[0] = allPoint[0] ++ centerPoint ++ " ")
      };
      (allPoint[0] = allPoint[0] ++ point ++ " ")
    }) |> ignore;
    <g> 
      <polyline 
        points=allPoint[0]
        stroke=(data.color === "" ? "black" : data.color)
        strokeWidth="3"
        fill=(data.color === "" ? "black" : data.color)
        style=(ReactDOMRe.Style.make(~opacity="0.4", ()))
      />
      {
        circlePoints |> Array.to_list |> List.map((p) => {
          <circle 
            cx=p[0]
            cy=p[1]
            r="5"
            stroke="none" fill=(data.color === "" ? "black" : data.color)
          />
        }) |> Array.of_list |> array
      }
    </g>
  }) |> Array.of_list |> array;
  
  let tooltipId = "tooltip-" ++ svgId;
  
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
      {dataElements}
    </svg>
    <div 
      id=tooltipId
      className="graph-tooltip"
    />
  </div>
};