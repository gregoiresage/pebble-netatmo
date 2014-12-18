function pushUInt8(array, value){
  array.push(value >> 0 & 0xFF);
}

function pushUInt16(array, value){
  if(value < 0){
    value = (-value - 1) & 0xFF;
    value = ~value;
  }
  array.push(value >> 0 & 0xFF);
  array.push(value >> 8 & 0xFF);
}

function pushUInt32(array, value){
  array.push(value >> 0 & 0xFF);
  array.push(value >> 8 & 0xFF);
  array.push(value >> 16 & 0xFF);
  array.push(value >> 24 & 0xFF);
}