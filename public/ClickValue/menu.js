var isopened = true;
$('.demo ').click((e) => {
        console.log(isopened);
        if (!isopened) {
            isopened = true;
            $('.sidemenu_parent').css('display', 'none');



        } else {
            $('.sidemenu_parent').css('display', 'flex');
            document.querySelector(".side_menu").classList.add('active');
            isopened = false;
        }

    })
    // $('.sidemenu_parent').click((e)=>{
    //     isopened = true;
    //     $('.sidemenu_parent').css('display', 'none');
    // })
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function(e) {
        e.preventDefault();

        document.querySelector(this.getAttribute('href')).scrollIntoView({
            behavior: 'smooth'
        });
    });
});
$('.right ul li').click((e) => {
    isopened = true;
    $('.sidemenu_parent').css('display', 'none');

})