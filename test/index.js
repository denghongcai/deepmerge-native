const merge = require('../');
const assert = require('assert');

describe('merge', function() {
  it('should add keys in target that do not exist at the root', function () {
    const src = { key1: 'value1', key2: 'value2' };
    const target = {};

    const res = merge(target, src);

    assert.deepEqual(target, {});
    assert.deepEqual(res, src);
  });

  it('merge existing simple keys in target at the roots', function () {
    const src = { key1: 'changed', key2: 'value2' };
    const target = { key1: 'value1', key3: 'value3' };

    const expected = {
      key1: 'changed',
      key2: 'value2',
      key3: 'value3'
    };

    assert.deepEqual(merge(target, src), expected);
    assert.deepEqual(target, { key1: 'value1', key3: 'value3' });
  });

  it('merge nested objects into target', function () {
    const src = {
      key1: {
        subkey1: 'changed',
        subkey3: 'added'
      }
    };
    const target = {
      key1: {
        subkey1: 'value1',
        subkey2: 'value2'
      }
    };

    const expected = {
      key1: {
        subkey1: 'changed',
        subkey2: 'value2',
        subkey3: 'added'
      }
    };

    assert.deepEqual(merge(target, src), expected);
    assert.deepEqual(target, {
      key1: {
        subkey1: 'value1',
        subkey2: 'value2'
      }
    });
  });

  it('replace simple key with nested object in target', function () {
    const src = {
      key1: {
        subkey1: 'subvalue1',
        subkey2: 'subvalue2'
      }
    }
    const target = {
      key1: 'value1',
      key2: 'value2'
    };

    const expected = {
      key1: {
        subkey1: 'subvalue1',
        subkey2: 'subvalue2'
      },
      key2: 'value2'
    };

    assert.deepEqual(merge(target, src), expected);
    assert.deepEqual(target, { key1: 'value1', key2: 'value2' });
  });

  it('should add nested object in target', function () {
    const src = {
      "b": {
        "c": {}
      }
    };

    const target = {
      "a": {}
    };

    const expected = {
      "a": {},
      "b": {
        "c": {}
      }
    };
  
    assert.deepEqual(merge(target, src), expected);
    assert.deepEqual(target, {a: {}});
  });

  it('should replace object with simple key in target', function () {
    const src = { key1: 'value1' };
    const target = {
      key1: {
        subkey1: 'subvalue1',
        subkey2: 'subvalue2'
      },
      key2: 'value2'
    };

    const expected = { key1: 'value1', key2: 'value2' };

    assert.deepEqual(merge(target, src), expected);
    assert.deepEqual(target, {
      key1: {
        subkey1: 'subvalue1',
        subkey2: 'subvalue2'
      },
      key2: 'value2'
    });
  });

  it('should work on simple array', function () {
    const src = ['one', 'three'];
    const target = ['one', 'two'];

    const expected = ['one', 'two', 'three'];

    assert.deepEqual(merge(target, src), expected);
    assert.ok(Array.isArray(merge(target, src)));
    assert.deepEqual(target, ['one', 'two']);
  });

  it('should work on another simple array', function () {
    const target = ["a1","a2","c1","f1","p1"];
    const src = ["t1","s1","c2","r1","p2","p3"];

    const expected = ["a1", "a2", "c1", "f1", "p1", "t1", "s1", "c2", "r1", "p2", "p3"];
    assert.deepEqual(merge(target, src), expected);
    assert.ok(Array.isArray(merge(target, src)));
    assert.deepEqual(target, ["a1", "a2", "c1", "f1", "p1"]);
  });

  it('should work on array properties', function () {
    const src = {
      key1: ['one', 'three'],
      key2: ['four']
    };
    const target = {
      key1: ['one', 'two']
    };

    const expected = {
      key1: ['one', 'two', 'three'],
      key2: ['four']
    };

    assert.deepEqual(merge(target, src), expected);
    assert.ok(Array.isArray(merge(target, src).key1));
    assert.ok(Array.isArray(merge(target, src).key2));
    assert.deepEqual(target, {
      key1: ['one', 'two']
    });
  });

  it('should work on array of objects', function () {
    const src = [
      { key1: ['one', 'three'], key2: ['one'] },
      { key3: ['five'] }
    ];
    const target = [
      { key1: ['one', 'two'] },
      { key3: ['four'] }
    ];

    const expected = [
      { key1: ['one', 'two', 'three'], key2: ['one'] },
      { key3: ['four', 'five'] }
    ];

    assert.deepEqual(merge(target, src), expected);
    assert.ok(Array.isArray(merge(target, src)), 'result should be an array');
    assert.ok(Array.isArray(merge(target, src)[0].key1), 'subkey should be an array too');
    assert.deepEqual(target, [
      { key1: ['one', 'two'] },
      { key3: ['four'] }
    ]);
  });

  it('should work on arrays of nested objects', function () {
    const target = [
      { key1: { subkey: 'one' }}
    ];

    const src = [
      { key1: { subkey: 'two' }},
      { key2: { subkey: 'three' }}
    ];

    const expected = [
      { key1: { subkey: 'two' }},
      { key2: { subkey: 'three' }}
    ];

    assert.deepEqual(merge(target, src), expected);
    assert.deepEqual(target, [{key1: {subkey: 'one'}}]);
  });
});
