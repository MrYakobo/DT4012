#!/usr/env/bin node

function encode(t){
    var i = Math.floor(t);
    var d = Math.round((t-i)*4);
    return String.fromCharCode((i<<2)+d)
}

function decode(t){
  t = t.charCodeAt(0)
  var i = t>>2;
  var d = (t&3)/4.0;
  return i+d;
}

if(process.argv[2] == "encode"){
    console.log(encode(process.argv[3]))
}
if(process.argv[2] == "decode"){
    console.log(decode(process.argv[3]))
}

console.log(process.argv)