const canv = document.getElementById('canv');
const ctx  = canv.getContext('2d');
const ws = new WebSocket('ws:/127.0.0.1:9000');

let img = ctx.createImageData(3520, 1080);
canv.width = img.width;
canv.height = img.height;

ws.onmessage = (evt)=>{
  let bin = atob(evt.data);
  let j = 0;
  for(let n=0;n<3520*1080*3;n+=3){
    img.data[j+0] = bin.charCodeAt(n+0);
    img.data[j+1] = bin.charCodeAt(n+1);
    img.data[j+2] = bin.charCodeAt(n+2);
    img.data[j+3] = 255;
    j += 4;
  }
  ctx.putImageData(img, 0, 0);
};
