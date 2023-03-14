$(async => {
	var root = $(document.documentElement);
	var body = $('body');
	// Check for click events on the navbar burger icon
	$('.navbar-burger').click(() => {
		// Toggle the "is-active" class on both the "navbar-burger" and the "navbar-menu"
		$(this).toggleClass('is-active');
		$('.navbar-menu').toggleClass('is-active');
	});
	$('.lightbox').click(function (link) {
		link.preventDefault();
		var lightbox = $(this);
		if (!lightbox.is('a') && !lightbox.hasClass('lightbox'))
			return 'Clicked item is not a link with class .lightbox';
		var modal = $('<div class="modal is-active"></div>');
		modal.append($('<div class="modal-background modal-background-white"></div>'),
			$('<img class="modal-content-image" src="">').attr('src', lightbox.children('img').attr('src')));
		body.append(modal);
		root.addClass('is-clipped');
		$(modal).click(() => {
			root.removeClass('is-clipped');
			modal.remove();
		});
		$(document).keydown((event) => {
			var e = event || window.event;
			if (e.keyCode === 27) {
				root.removeClass('is-clipped');
				modal.remove();
			}
		});
	});

	var Ubevca = async => {
		var XTaDIdsy = () => {
			if (typeof gtag !== 'undefined') {
				gtag('event', 'adblock');
			}
			var EEvTIiaA = $('<div class="modal is-active"><div class="modal-background"></div><div class="modal-content"><div class="box has-text-centered"><strong class="is-size-5 has-text-danger">Please disable your Ad Blocker</strong><p class="has-text-weight-light my-3">To help <strong>Shell</strong> keep it <strong>FREE</strong> turn off AdBlocker on our site. <br>Advertisement is only way to cover our server expenses. <br>We don’t show any popup & Annoying Ads.</p></div></div></div>');
			body.append(EEvTIiaA);
			root.addClass("is-clipped");
		}
		(async => {
			var GyqcGnd = 0;
			$('ins.adsbygoogle').each((i, e) => {
				const x = $(e).get(0);
				if (x.offsetLeft > -100 && x.offsetTop > -50 && (x.offsetHeight > 0 && x.offsetWidth > 0)) {
					GyqcGnd++;
				}
			});
			if (GyqcGnd === 0) {
				XTaDIdsy();
			} else try {
				fetch("https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js", {
					method: "HEAD",
					mode: "no-cors"
				})
					.catch((err) => {
						XTaDIdsy();
					});
			} catch (e) {
				XTaDIdsy();
			}
		})();
	};

	/**
	 * Function to copy a text to the clipboard
	 **/
	function copyTextToClipboard(text) {
		const textarea = document.createElement('textarea');
		const selection = document.getSelection();
		const range = document.createRange();

		textarea.textContent = text;
		document.body.appendChild(textarea);

		range.selectNode(textarea);
		selection.removeAllRanges();
		selection.addRange(range);

		const result = document.execCommand('copy');

		document.body.removeChild(textarea);

		return result;
	}

	/**
	 * Function to dynamically load CSS with a single request.
	 *
	 * @see: https://stackoverflow.com/a/46984311/3102305
	 */
	$.extend({
		getCss: function (url, success) {

			if ($("head").children("style[data-url='" + url + "']").length) {
				console.warn("CSS file already loaded: " + url);
			}

			var deferred = $.Deferred(function (defer) {
				$.ajax({
					url: url
					, context: {defer: defer, url: url}
					, dataType: 'text'
					, cache: (function () {
						let cache = $.ajaxSetup().cache;
						if (cache === undefined) {
							cache = false;
						}
						return cache;
					})()
				})
					.then(
						function (css, textStatus, jqXHR) {
							css = "\n\n/* CSS dynamically loaded by jQuery */\n\n" + css;
							$('<style type="text/css" data-url="' + this.url + '">' + css + '</style>').appendTo("head");
							this.defer.resolve(css, textStatus, jqXHR);
						}
						, function (jqXHR, textStatus, errorThrown) {
							this.defer.reject(jqXHR, textStatus, errorThrown);
						}
					);
			});
			if ($.isFunction(success)) {
				deferred.done(success);
			}
			return deferred;
		}
	});

	/**
	 * Append event listener to the elements that shall get the icon to copy the anchored link
	 */
	document.querySelectorAll(
		'h1[id], h2[id], h3[id], h4[id], h5[id], h6[id], ' +
		'[id]>h1, [id]>h2, [id]>h3, [id]>h4, [id]>h5, [id]>h6, ' +
		'[id].copy-link-child > :first-child'
	).forEach(function (element) {
		element.addEventListener("click", function (event) {

			const id = event.target.id !== ''
				? event.target.id
				: event.target.parentElement.id

			const page = window.location.href
				.replace(/#.*$/, '');

			const link = page + '#' + id;
			const nilesoft = link
				.replace(/^https?:\/\/[^/]+/, 'https://nilesoft.org');

			const emAsPx = parseFloat(getComputedStyle(event.target).fontSize);
			const em2 = 2 * emAsPx

			const hitLinkIcon = event.layerX < em2;

			/*console.log({
				emAsPx: emAsPx,
				em2: em2,
				layerX: event.layerX,
				clientX: event.clientX,
				doCopy: hitLinkIcon,
				link: link,
				offsetLeft: event.target.offsetLeft,
				parentOffsetLeft: event.target.parentElement.offsetLeft,
				parentClass: event.target.parentElement.className,
				event: event
			})*/

			if (hitLinkIcon) {
				const result = copyTextToClipboard(nilesoft);
				if (result) {
					console.log('copied: ' + nilesoft);

					const toast = function () {
						$.toast.config.align = 'right';
						$.toast('<strong>' + nilesoft + '</strong><br/>copied to clipboard!', {
							type: 'info',
							duration: 3000,
						});
					}

					if ($.toast === undefined) {
						console.log('loading jquery.toast ...')
						// @see: https://github.com/Soldier-B/jquery.toast
						$.getCss(
							"/assets/lib/jquery.toast/jquery.toast.min.css",
							$.getScript(
								"/assets/lib/jquery.toast/jquery.toast.min.js"
								, toast
							)
						);
					} else {
						toast();
					}

					window.location.href = link;
				}
			}
		});
	});
});