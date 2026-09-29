// modify items
// Remove items by identifiers
modify(mode=mode.multiple
	where=this.id(id.restore_previous_versions,id.cast_to_device)
	vis=vis.remove)

modify(type="recyclebin" where=window.is_desktop and this.id==id.empty_recycle_bin pos=1 sep)

modify(where=this.id(id.unpin_from_start, id.unpin_from_taskbar, id.unpin_from_quick_access, id.unpin_from_start_menu) || find="unpin*|*取消固定*" pos="bottom" menu=loc.pin_unpin)
modify(where=this.id(id.pin_to_start, id.pin_to_taskbar, id.pin_to_quick_access, id.pin_to_start_menu, id.pin_current_folder_to_quick_access) || find="pin*|*固定*" pos="top" menu=loc.pin_unpin)

modify(where=this.id==id.copy_as_path menu=loc.file_manage)
modify(type="dir.back|drive.back" where=this.id==id.customize_this_folder pos=1 sep="top" menu=loc.file_manage)

modify(where=str.equals(this.name, ["open in terminal", "open linux shell here", "在终端中打开", "在終端機中開啟"]) || this.id==id.open_powershell_window_here
	pos="bottom" menu=title.terminal)

modify(mode=mode.multiple
	where=this.id(
		id.send_to,
		id.share,
		id.create_shortcut,
		id.set_as_desktop_background,
		id.rotate_left,
		id.rotate_right,
		id.map_network_drive,
		id.disconnect_network_drive,
		id.format,
		id.eject,
		id.give_access_to,
		id.include_in_library,
		id.print
	)
	pos=1 menu=title.more_options)
