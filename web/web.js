window.onerror = (a, b, c)=>{
  alert(a + b + c);
};

window.addEventListener('DOMContentLoaded', ()=>{
  const canv = document.getElementById('canv');
  const ctx  = canv.getContext('2d');
  const ws = new WebSocket('ws:/192.168.1.70:9000');

  let img,
      box_x = 0,
      box_y = 0;
  ws.onmessage = (evt)=>{
    if(evt.data.indexOf('size') > -1){
      canv.width = parseInt(evt.data.split('size')[1].split(',')[0]);
      canv.height = parseInt(evt.data.split('size')[1].split(',')[1]);

      let block_size = parseInt(evt.data.split('size')[1].split(',')[2]);
      img = ctx.createImageData(block_size, block_size);
    } else if(evt.data == 'newline'){
      box_x = 0;
      box_y += 1;
    } else {
      let bin = pako.inflate(atob(evt.data));
      let n = 0, i = 0;
      for(let y=0;y<img.height;y++){
        for(let x=0;x<img.width;x++){
          i = y * (img.width * 4) + x * 4;
          img.data[i+0] = bin[i+2]||bin[2];
          img.data[i+1] = bin[i+1]||bin[1];
          img.data[i+2] = bin[i+0]||bin[0];
          img.data[i+3] = 255;
        }
      }
      ctx.putImageData(img, img.width*box_x, img.height*box_y);
      box_x += 1;
    }
  };
});
