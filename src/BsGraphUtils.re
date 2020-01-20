open GraphDataTypes;

let floatToPrecision = (number: float, precision: int) => {
    let floatString = Js.Float.toFixedWithPrecision(number, ~digits=precision);
    let splitNumber = Js.String.split(".", floatString);
    let re = Js.Re.fromStringWithFlags("\\B(?=(\\d{3})+(?!\\d))", ~flags="g");
    let commaNumber = Js.String.replaceByRe(re, ",", splitNumber[0]);
    let result =
        switch (precision) {
        | 0 => commaNumber
        | _ => commaNumber ++ "." ++ splitNumber[1]
        };
    result;
};

let getDateStr = (~separateStr="/", date:float) => {
  let dateType = Js.Date.fromFloat(date);
  (((Js.Date.getMonth(dateType) +. 1.)) >= 10. ? 
    ((Js.Date.getMonth(dateType) +. 1.) |> Js.Float.toString) 
    : "0" ++ ((Js.Date.getMonth(dateType) +. 1.) |> Js.Float.toString)) ++ separateStr ++ 
  (Js.Date.getDate(dateType)  >= 10. ? 
    (Js.Date.getDate(dateType) |> Js.Float.toString)
    : "0" ++ (Js.Date.getDate(dateType) |> Js.Float.toString)) ++ separateStr ++
  (Js.Date.getFullYear(dateType) |> Js.Float.toString);
};

let pointFromPercent = (~startPoint, ~length, ~isX=false, percent) => startPoint +. ((length /. 100.) *. (isX ? percent : (100. -. percent)));
let percentOfData = (~data, ~maxValue) => (maxValue === 0. ? 0. : (data /. maxValue) *. 100.);

let sortDatetimeLineGraph = (lineGraphDatas:list(lineGraph)) => {
  lineGraphDatas |> List.map((lineGraphData) => {
    ()
  });
};

let lastPoint = (values, points, maxX) => {
    (List.length(values) === 1 ?
      (maxX |> Js.Float.toString) ++ "," ++ Js.String.split(",", points)[1]
      : (maxX |> Js.Float.toString) ++ "," 
          ++ Js.String.split(",", points)[List.length((Js.String.split(",", points) |> Array.to_list)) - 1]
    )
};

let filterByMonth = (month: int) =>
  Js.Date.setMonth(
    Js.Date.fromFloat(Js.Date.now()),
    Js.Date.getMonth(Js.Date.fromFloat(Js.Date.now()))
    -. float_of_int(month),
);

let filterByYear = (year: int) =>
  Js.Date.setFullYear(
    Js.Date.fromFloat(Js.Date.now()),
    Js.Date.getFullYear(Js.Date.fromFloat(Js.Date.now()))
    -. float_of_int(year),
);

let getXAxisAsTimes = timelimit => {
    GraphDataTypes.(
      switch (timelimit) {
      | OneMonth
      | ThreeMonths =>
        let axisAsTimes = [||];
        for (i in 2 downto 0) {
          Js.Array.push(
            Js.Date.setDate(filterByMonth(i) |> Js.Date.fromFloat, 1.),
            axisAsTimes,
          )
          |> ignore;
  
          Js.Array.push(
            Js.Date.setDate(filterByMonth(i) |> Js.Date.fromFloat, 15.),
            axisAsTimes,
          )
          |> ignore;
        };
        axisAsTimes |> Array.to_list;
      | OneYear =>
        let axisAsTimes = [||];
        for (i in 15 downto 0) {
          Js.Array.push(
            Js.Date.setDate(filterByMonth(i) |> Js.Date.fromFloat, 1.),
            axisAsTimes,
          )
          |> ignore;
        };
        axisAsTimes |> Array.to_list;
      | _ => []
      }
    );
};

let drawGuildLineX = (~lineAmount=20, ~positionPoints, ~strokeColor) => {
    let lines = [||];
    for (i in lineAmount downto 0) {
      let y =
        positionPoints.minY
        +. (positionPoints.maxY -. positionPoints.minY)
        /. (lineAmount |> float_of_int)
        *. (i |> float_of_int);
      Js.Array.push(
        <line
          x1={floatToPrecision(positionPoints.minX, 2)}
          y1={floatToPrecision(y, 2)}
          x2={floatToPrecision(positionPoints.maxX, 2)}
          y2={floatToPrecision(y, 2)}
          strokeWidth="0.5"
          stroke=strokeColor
        />,
        lines,
      )
      |> ignore;
    };
    lines |> ReasonReact.array;
};

let drawXvaluesStr = (~positionPoints, ~fontColor, ~fontSize=30., ~dateStart:float, ~dateEnd:float, ~range) => {
  let yValues = [||];
  for (i in range downto 0) {
    let dateStr = (dateStart +. (((dateEnd -. dateStart) /. (range |> float_of_int)) *. (i |> float_of_int))) |> getDateStr;
    let x =
      (positionPoints.minX
      +. (positionPoints.maxX -. positionPoints.minX)
      /. (range |> float_of_int)
      *. (i |> float_of_int))  -. fontSize -. 
      ((Js.String.length(dateStr) |> float_of_int) *. (fontSize /. 6.5));
    
    Js.Array.push(
      <text 
        key=("text-"++(i |> string_of_int)++"-"++dateStr)
        x=floatToPrecision(x, 2)
        y=floatToPrecision(positionPoints.maxY +.  (fontSize *. 2.), 2)
        fill=fontColor
        fontSize=(floatToPrecision(fontSize, 2)++"px")
      >
        {ReasonReact.string(dateStr)}
      </text>,
      yValues,
    )
    |> ignore;
  };
  yValues |> ReasonReact.array;
};

let floatDigitToString = (~value:float, ~floatDigit) => {
  (value <= 0. ? floatToPrecision(value, floatDigit) : ("+" ++ (floatToPrecision(value, floatDigit))))
};

let drawYvalues = (~yValueLength, ~yLength, ~boundary, ~floatDigit, ~colorElements, ~fontSize) => {
  open ReasonReact;
  let yValueDistance = (yValueLength /. ((yLength |> float_of_int) -. 1.));
  let yPxDistance = ((boundary.positionPoints.maxY -. boundary.positionPoints.minY) /. ((yLength |> float_of_int) -. 1.));
  Array.make((yLength - 1), ReasonReact.null)
  |> Array.mapi((idx, _) => {
    let value = boundary.yValue.min +. (yValueDistance *. ((idx) |> float_of_int));
    let lastValue = boundary.yValue.min +. (yValueDistance *. ((idx + 1) |> float_of_int));
    let yValueStr = floatDigitToString(~value, ~floatDigit);
    let y = boundary.positionPoints.maxY -. (yPxDistance *. ((idx) |> float_of_int));
    let lastY = boundary.positionPoints.maxY -. (yPxDistance *. ((idx + 1) |> float_of_int));
    <g key=("g-yvalue-" ++ (yValueStr))>
      <text 
        x=(floatToPrecision(boundary.positionPoints.minX -. (fontSize *. 2.) -. 
          ((Js.String.length(yValueStr) |> float_of_int) *. (fontSize /. 3.5)), 2))
        y=floatToPrecision(y +. (fontSize /. 2.5), 2)
        fill=colorElements.font
        fontSize=(floatToPrecision(fontSize, 2)++"px")
      >
        {string(yValueStr)}
      </text>
      (idx === (yLength - 2) ? 
        <text 
          x=(floatToPrecision(boundary.positionPoints.minX -. (fontSize *. 2.) -. 
            ((Js.String.length(floatDigitToString(~value=lastValue, ~floatDigit)) |> float_of_int) *. (fontSize /. 3.5)), 2))
          y=floatToPrecision(lastY +. (fontSize /. 2.5), 2)
          fill=colorElements.font
          fontSize=(floatToPrecision(fontSize, 2)++"px")
        >
          {string(floatDigitToString(~value=lastValue, ~floatDigit))}
        </text> : null
      )
    </g>
  }) |> array
};

let defaultGraphSize = {
  width: 1000,
  height: 500,
};

let defaultPositionPoints = {
  minX: 100.,
  maxX: 900.,
  minY: 50.,
  maxY: 400.,
};

let defaultYValue = {
  max: 10.,
  min: -10.,
};

let defaultBoundary = {
  graphSize: defaultGraphSize,
  positionPoints: defaultPositionPoints,
  yValue: defaultYValue,
};

let defaultColors = {
  font: "black",
  guildLines: "gray",
  axisLine: "black",
  border: "black",
};

let defaultDisabledElements = {
  dataLines: false,
  dataArea: false,
  dataPoints: false,
  guildLines: false,
  border: false,
  middleLine: false,
};

let hideTooltip: (string, string) => unit = [%bs.raw
  {|
    function(tooltipId, circleId) {
      if (document.getElementById(tooltipId)) {
        document.getElementById(tooltipId).style.display = "none";
      }
      if (document.getElementById(circleId)) {
        document.getElementById(circleId).style.display = "none";
      }
    }
  |}
];
