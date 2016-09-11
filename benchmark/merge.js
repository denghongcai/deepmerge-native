const Benchmark = require('benchmark');
const suite = new Benchmark.Suite;
const _ = require('lodash');
const merge = require('../');

const src = {a: {a: 1}, b: {a: 1, c: {a: 2, b: 1}}, c: [2, {b: 2}]};
const target = {a: {b: 1}, b: {a: 1, c: {a: 1}}, c: [1, {a: 1}]};;

suite
.add('merge', function() {
  merge(target, src);
})
.add('lodash#merge, with cloneDeep', function() {
  _.merge(_.cloneDeep(target), src);
})
.on('cycle', function(event) {
  console.log(String(event.target));
})
.on('complete', function() {
  console.log('Fastest is ' + this.filter('fastest').map('name'));
})
.run({
  async: true,
  minSamples: 20000
});
