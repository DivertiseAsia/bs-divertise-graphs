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
  let (_state, dispatch) = React.useReducer(
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

  let values1:list(point) = [
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
  let values2:list(point) = [
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
      points: values1,
    },
    {
      title: "data2",
      color: "blue",
      points: values2,
    },
  ];
  
let categoryGraph:list(categoryGraph) = [
    {
      title: "b1",
      color: "black",
      point: {
        x: 0., 
        y: 9.5
      },
    }, 
    {
      title: "b1",
      color: "black",
      point: {
        x: 0., 
        y: 7.3
      },
    }, 
    {
      title: "b2",
      color: "red",
      point: {
        x: 0., 
        y: 4.5
      },
    },
    {
      title: "b33333333333",
      color: "green",
      point: {
        x: 0., 
        y: -4.5
      },
    },  
    {
      title: "b3",
      color: "green",
      point: {
        x: 0., 
        y: -4.5
      },
    }, 
  ];
  <>
    <CategoryGraph 
      svgId="category-graph" 
      datas=categoryGraph
    />
    <LineGraph 
      svgId="line-graph" 
      datas
    />
  </>
};
