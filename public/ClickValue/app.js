let circle = document.getElementById('circle');
var y;
var x;

const updatePosition = () => {
    circle.style.transform = "translate(" + x + "px, " + y + "px)";
};

const onMouseMove = (e) => {
    const scrollX = window.scrollX || window.pageXOffset; // Horizontal scroll position
    const scrollY = window.scrollY || window.pageYOffset; // Vertical scroll position
    x = e.pageX - circle.clientWidth / 2 - scrollX;
    y = e.pageY - circle.clientHeight / 2 - scrollY;
    updatePosition();
};

document.addEventListener('mousemove', onMouseMove);
