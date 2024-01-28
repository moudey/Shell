#pragma once

namespace Nilesoft
{
	namespace Shell
	{
		class Keyboard
		{
			uint8_t _KEYS[128] =
			{
				{0x01},{0x02},{0x03},{0x04},{0x05},{0x06},{0x08},{0x09},{0x0C},{0x0D},{0x13},{0x14},{0x15},{0x16},{0x17},{0x18},
				{0x19},{0x1A},{0x1B},{0x1C},{0x1D},{0x1E},{0x1F},{0x20},{0x21},{0x22},{0x23},{0x24},{0x25},{0x26},{0x27},{0x28},
				{0x29},{0x2A},{0x2B},{0x2C},{0x2D},{0x2E},{0x2F},{0x30},{0x31},{0x32},{0x33},{0x34},{0x35},{0x36},{0x37},{0x38},
				{0x39},{0x41},{0x42},{0x43},{0x44},{0x45},{0x46},{0x47},{0x48},{0x49},{0x4A},{0x4B},{0x4C},{0x4D},{0x4E},{0x4F},
				{0x50},{0x51},{0x52},{0x53},{0x54},{0x55},{0x56},{0x57},{0x58},{0x59},{0x5A},{0x5B},{0x5C},{0x5D},{0x5F},{0x60},
				{0x61},{0x62},{0x63},{0x64},{0x65},{0x66},{0x67},{0x68},{0x69},{0x6A},{0x6B},{0x6C},{0x6D},{0x6E},{0x6F},{0x70},
				{0x71},{0x72},{0x73},{0x74},{0x75},{0x76},{0x77},{0x78},{0x79},{0x7A},{0x7B},{0x7C},{0x7D},{0x7E},{0x7F},{0x80},
				{0x81},{0x82},{0x83},{0x84},{0x85},{0x86},{0x87},{0x90},{0x91},{0x92},{0xA0},{0xA1},{0xA2},{0xA3},{0xA4},{0xA5}
			};

			std::vector<uint8_t> _keys;
			uint8_t _has_contextmenu = 0;

		public:
			Keyboard(bool init = false)
			{
				reset();
				if(init) get_keys_state();
			}

			void reset()
			{
				_keys.clear();
			}
		
			uint8_t get_keys_state(bool async = true)
			{
				reset();

				auto swb = IsSwapButton();

				for(auto key : _KEYS)
				{
					if(IsKeyDown(key, async))
					{
						if(key == VK_LBUTTON && swb)
							_has_contextmenu = VK_LBUTTON;
						else if(key == VK_RBUTTON && !swb)
							_has_contextmenu = VK_RBUTTON;
						_keys.push_back(key);
					}
				}

				if(!_has_contextmenu && key_shift() && key(VK_F10))
					_has_contextmenu = VK_SHIFT + VK_F10;
				return count();
			}

			uint8_t get_keys_excloude_contextmenu()
			{
				reset();
				auto swb = IsSwapButton();
				for(auto key : _KEYS)
				{
					if(IsKeyDown(key, true))
					{
						if(key == VK_LBUTTON && swb)
						{
							_has_contextmenu = VK_LBUTTON;
							continue;
						}
						else if(key == VK_RBUTTON && !swb)
						{
							_has_contextmenu = VK_RBUTTON;
							continue;
						}
						_keys.push_back(key);
					}
				}

				if(!_has_contextmenu)
				{
					if(key_shift() && key(VK_F10))
					{
						_has_contextmenu = VK_SHIFT + VK_F10;
						for(auto it = _keys.begin(); it != _keys.end(); it++)
						{
							if(*it == VK_LSHIFT || *it == VK_RSHIFT || *it == VK_F10)
								_keys.erase(it--);
						}
					}
				}
				return count();
			}

			uint8_t count() const { return static_cast<uint8_t>(_keys.size()); }
			uint8_t first() const { return count() == 0 ? 0 : _keys.front(); }
			uint8_t last() const { return count() == 0 ? 0 : _keys.back();}

			Keyboard& add_key(uint8_t key)
			{
				for(const auto k : _keys) {
					if(k == key)
						return *this;
				}
				_keys.push_back(key);
				return *this;
			}

			bool key(uint8_t key) const
			{
				if(count() > 0)
				{
					for(auto k : _keys) if(k == key) return true;
				}
				return false;
			}

			bool key(std::initializer_list<uint8_t> keys) const
			{
				for(auto k : keys) if(key(k)) return true;
				return false;
			}

			bool equals(std::initializer_list<uint8_t> keys) const
			{
				if(keys.size() == count())
				{
					for(auto k : keys)
					{
						if(!key(k)) return false;
					}
					return true;
				}
				return false;
			}

			bool key_shift() const { return key({ VK_LSHIFT, VK_RSHIFT }); }
			bool key_lshift() const { return key(VK_LSHIFT); }
			bool key_srhift() const { return key(VK_RSHIFT); }

			bool key_ctrl() const { return key({ VK_LCONTROL, VK_RCONTROL }); }
			bool key_lctrl() const { return key(VK_LCONTROL); }
			bool key_rctrl() const { return key(VK_RCONTROL); }

			bool key_alt() const { return key({ VK_LMENU, VK_RMENU }); }
			bool key_lalt() const { return key(VK_LMENU); }
			bool key_ralt() const { return key(VK_RMENU); }

			bool key_win() const { return key({ VK_LWIN, VK_RWIN }); }
			bool key_lwin() const { return key(VK_LWIN); }
			bool key_rwin() const { return key(VK_RWIN); }

			bool has_contextmenu_func() const { return _has_contextmenu; }

			bool is_contextmenu() const { return key_shift() && key(VK_F10); }
			bool is_rbutton() const { return key(VK_RBUTTON); }
			bool is_lbutton() const { return key(VK_LBUTTON); }
			bool is_mbutton() const { return key(VK_MBUTTON); }

			uint8_t *begin() { return _keys.data(); }
			const uint8_t *begin() const { return begin(); }

			uint8_t *end() { return _keys.data() + _keys.size(); }
			const uint8_t *end() const { return end(); }

			static bool IsKeyDown(const std::initializer_list<uint8_t> &keys, bool async = true)
			{
				for(auto k : keys) if(IsKeyDown(k, async)) return true;
				return false;
			}

			//is_pressed
			static bool IsKeyDown(uint8_t key, bool async = true) {
				return (0x8000 & (async ? ::GetAsyncKeyState(key) : ::GetKeyState(key))) == 0x8000;
			}

			static bool IsSwapButton()
			{
				return ::GetSystemMetrics(SM_SWAPBUTTON);
			}
		};
	}
}