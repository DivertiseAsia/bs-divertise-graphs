type timeRange =
  | Month(int)
  | Year(int)
  | AllTime;

type point = {
    x: float,
    y: float,
};

type lineGraph = {
    title: string,
    color: string,
    points: list(point),
};

type categoryGraph = {
  title: string,
  color: string,
  point,
};

type disabledElements = {
  dataLines: bool,
  dataArea: bool,
  dataPoints: bool,
  guildLines: bool,
  border: bool,
  middleLine: bool,
};

type colorElements = {
  font: string,
  guildLines: string,
  axisLine: string,
  border: string,
};

type graphSize = {
  width: int,
  height: int,
};

type positionPoints = {
  minX: float,
  maxX: float,
  minY: float,
  maxY: float,
};

type yValue = {
  max: float,
  min: float,
};

type boundaryGraph = {  
  graphSize,
  positionPoints,
  yValue,
};

type tooltipTitle = {
  xTitle: string,
  yTitle: string,
};
