

function resizefull(canvas) {
    var displayWidth = window.innerWidth;
    var displayHeight = window.innerHeight;
    if (canvas.width != displayWidth) {
        canvas.width = displayWidth;
    }
    if (canvas.height != displayHeight) {
        canvas.height = displayHeight;
    }
}


async function copyToClipboard(text) {
    try {
      await navigator.clipboard.writeText(text);
      console.log('クリップボードにコピーしました');
    } catch (err) {
      console.error('クリップボードへのコピーに失敗しました:', err);
    }
  }
  
  async function copyToClipboard(text) {
    try {
      await navigator.clipboard.writeText(text);
      console.log('クリップボードにコピーしました');
    } catch (err) {
      console.error('クリップボードへのコピーに失敗しました:', err);
    }
  }
  
 function hello() {

    const textToCopy = 'コピーしたいテキスト111';
    copyToClipboard(textToCopy);
    var canvas = document.querySelector("#can");
    resizefull(canvas);
    const ctx = canvas.getContext("2d");
    ctx.fillStyle = "rgba(" + [0, 0, 255, 0.5] + ")";
    ctx.fillRect(0, 0, 150, 100);    
}