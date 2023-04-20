shell
{
	// static items

	// Delete items by identifiers
	item(mode=mode.multiple
		where=this.id(id.restore_previous_versions,id.cast_to_device)
		vis=vis.remove)
		
	item(type='recyclebin' where=window.is_desktop and this.id==id.empty_recycle_bin pos=1 sep)
	item(type='back' find=['shortcut', '/new'] vis=vis.remove)
	item(find='unpin' pos=pos.bottom menu="Pin//Unpin")
	item(find='pin' pos=pos.top menu="Pin//Unpin")
	item(where=this.id==id.copy_as_path menu='file manage')
	item(type='dir.back|drive.back' where=this.id==id.customize_this_folder pos=1 sep='top' menu='file manage')
	item(find='open in terminal*' pos=pos.bottom sep menu='Terminal')
	item(find='open with visual studio' pos=1 menu='develop/editors')
	//Move and organize
	//item(mode=mode.multiple find='scan with' menu=title.more_options)
	item(mode=mode.multiple
		where=this.id(id.send_to,id.share,id.create_shortcut,id.set_as_desktop_background,id.rotate_left,
						id.rotate_right, id.map_network_drive,id.disconnect_network_drive,id.format, id.eject,
						id.give_access_to,id.include_in_library,id.print)
		pos=1 menu=title.more_options)
}