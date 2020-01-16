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

type valueLineGraph = {
    x: float, /* timestamp */
    y: float, /* value */
};

type lineGraph = {
    title: string,
    color: string,
    values: list(valueLineGraph),
};
