type timeRange =
  | OneMonth
  | TwoMonths
  | ThreeMonths
  | FourMonths
  | FiveMonths
  | SixMonths
  | SevenMonths
  | EightMonths
  | NineMonths
  | TenMonths
  | ElevenMonths
  | OneYear
  | AllTime;

type point = {
    x: float, /* timestamp */
    y: float, /* value */
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
