type timelimits =
  | OneMonth
  | ThreeMonths
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
