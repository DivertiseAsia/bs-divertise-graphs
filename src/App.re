open ReasonReact;
open GraphDataTypes;
[@bs.val] external encodeURIComponent: string => string = "";
[@bs.get] external location: Dom.window => Dom.location = "";
[@bs.get] external href: Dom.location => string = "";
type dom;
[@bs.send]
external addEventListener : (dom, string, unit => unit) => unit =
  "addEventListener";
[@bs.val] external dom: dom = "document";
type state = {
  route: ReasonReact.Router.url,
  deviceReady: bool
};

type action =
  | RouteTo(Router.url)
  | OnDeviceReady;

let routeMatches = (x, link) => "/" ++ x == link;

[@react.component]
let make = () => {
  let (state, dispatch) = React.useReducer(
    (state, action) =>
      switch (action) {
        | RouteTo(route) => {...state, route: route}
        | OnDeviceReady => {...state, deviceReady: true}
      },
    {route: Router.dangerouslyGetInitialUrl(), deviceReady: false}
  );
  React.useEffect0(() => {
    let _ = addEventListener(dom, "deviceready", (_) => {
      dispatch(OnDeviceReady);
    });
    let watcherID = Router.watchUrl(url => dispatch(RouteTo(url)));
    Some(() => Router.unwatchUrl(watcherID));
  });

  let values1:list(valueLineGraph) = [
    {
      x: Js.Date.now() -. 1500000000., /* timestamp */
      y: 8.4, /* value */
    },
    {
      x: Js.Date.now() -. 50000000., /* timestamp */
      y: 5.4, /* value */
    },
    {
      x: Js.Date.now(), /* timestamp */
      y: 6.1, /* value */
    },
  ];
  let values2:list(valueLineGraph) = [
    {
      x: Js.Date.now() -. 1500000000., /* timestamp */
      y: -5.7, /* value */
    },
    {
      x: Js.Date.now() -. 600000000., /* timestamp */
      y: -3.9, /* value */
    },
    {
      x: Js.Date.now(), /* timestamp */
      y: 3.2, /* value */
    },
  ];

  let datas:list(lineGraph) = [
    {
      title: "data1",
      color: "red",
      values: values1,
    },
    {
      title: "data2",
      color: "blue",
      values: values2,
    },
  ];
  <LineGraph 
    svgId="line-graph" 
    datas
  />
};
