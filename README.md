# deepmerge-native

A very approachable node native extension.

This repository serves as a nearly minimal native extension built on [Nan](https://github.com/nodejs/nan) with enough tooling to also make it a great starting point for more complex projects.

## Building

To compile the extension for the first time, run 

```
$ npm i
$ npm run configure
$ npm run build
```

All subsequent builds only need `npm run build`

You can confirm everything built correctly by [running the test suite](#to-run-tests).

### Working With the Extension Locally

After building:

```node
$ node
> var merge = require('./')
undefined
> merge({a: 1, c: 3}, {a: 2, b: 1})
{a: 2, b: 1, c: 3}
```

### To run tests:

```
$ npm test
```

or to run test continuously 

```
$ npm test -- watch
```

## The Parts

File | Contents
-------------|----------------
`NativeExtension.cc` | Represents the top level of the module. C++ constructs that are exposed to javascript are exported here
`functions.cc` | top-level functions. These functions demonstrate how to build and return various js types.
`index.js` | The main entry point for the node dependency
`binding.gyp` | Describes your node native extention to the build system (`node-gyp`). As you add source files to the project, you should also add them to the binding file.
