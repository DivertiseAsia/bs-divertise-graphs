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

let drawGuildLineX = (~lineAmount=20, ~minX, ~maxX, ~minY, ~maxY, ~strokeColor) => {
    let lines = [||];
    for (i in lineAmount downto 0) {
      let y =
        minY
        +. (maxY -. minY)
        /. (lineAmount |> float_of_int)
        *. (i |> float_of_int);
      Js.Array.push(
        <line
          x1={floatToPrecision(minX, 2)}
          y1={floatToPrecision(y, 2)}
          x2={floatToPrecision(maxX, 2)}
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

let drawXvaluesStr = (~minX, ~maxX, ~maxY, ~fontColor, ~fontSize=30., ~dateStart:float, ~dateEnd:float, ~range) => {
  let yValues = [||];
  for (i in range downto 0) {
    let dateStr = (dateStart +. (((dateEnd -. dateStart) /. (range |> float_of_int)) *. (i |> float_of_int))) |> getDateStr;
    let x =
      (minX
      +. (maxX -. minX)
      /. (range |> float_of_int)
      *. (i |> float_of_int))  -. fontSize -. 
      ((Js.String.length(dateStr) |> float_of_int) *. (fontSize /. 6.5));
    
    Js.Array.push(
      <text 
        x=floatToPrecision(x, 2)
        y=floatToPrecision(maxY +.  (fontSize *. 2.), 2)
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
