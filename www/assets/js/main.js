$(async => {
	var root = $(document.documentElement);
	var body = $('body');
	// Check for click events on the navbar burger icon
	$('.navbar-burger').click(()=> {
		// Toggle the "is-active" class on both the "navbar-burger" and the "navbar-menu"
		$(this).toggleClass('is-active');
		$('.navbar-menu').toggleClass('is-active');
	});
	$('.lightbox').click(function(link) {
		link.preventDefault();
		var lightbox = $(this);
		if(!lightbox.is('a') && !lightbox.hasClass('lightbox'))
			return 'Clicked item is not a link with class .lightbox';
		var modal = $('<div class="modal is-active"></div>');
		modal.append($('<div class="modal-background modal-background-white"></div>'), 
			$('<img class="modal-content-image" src="">').attr('src',lightbox.children('img').attr('src')));
		body.append(modal);
		root.addClass('is-clipped');
		$(modal).click(()=>{
			root.removeClass('is-clipped');
			modal.remove();
		});
		$(document).keydown((event)=> {
			var e = event || window.event;
			if (e.keyCode === 27) {
				root.removeClass('is-clipped');
				modal.remove();
			}
		});
	});
	var Ubevca = async => {
		var XTaDIdsy = ()=> {
			if(typeof gtag !=='undefined'){gtag('event', 'adblock');}
			var EEvTIiaA = $('<div class="modal is-active"><div class="modal-background"></div><div class="modal-content"><div class="box has-text-centered"><strong class="is-size-5 has-text-danger">Please disable your Ad Blocker</strong><p class="has-text-weight-light my-3">To help <strong>Shell</strong> keep it <strong>FREE</strong> turn off AdBlocker on our site. <br>Advertisement is only way to cover our server expenses. <br>We don’t show any popup & Annoying Ads.</p></div></div></div>');
			body.append(EEvTIiaA);
			root.addClass("is-clipped");
		}
		(async => {
			var GyqcGnd = 0;
			$('ins.adsbygoogle').each((i,e) => {
				const x = $(e).get(0);
				if(x.offsetLeft > -100 && x.offsetTop > -50 && (x.offsetHeight > 0 && x.offsetWidth > 0)) {
					GyqcGnd++;
				}
			});
			if(GyqcGnd === 0) {
				XTaDIdsy();
			} else try {
				fetch("https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js",{ method: "HEAD", mode: "no-cors" })
					.catch((err) => {XTaDIdsy();});
			} catch(e) {
				XTaDIdsy();
			}
		})();
	};
});