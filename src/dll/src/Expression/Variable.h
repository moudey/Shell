#pragma once
namespace Nilesoft
{
	namespace Shell
	{
		/*class Variable
		{
		public:
			uint32_t Id = 0;
			Expression *Value = nullptr;
			std::vector<Variable> Items;

			Variable() {}
			Variable(Variable &&other) noexcept { this->operator=( std::move(other)); }

			Variable(uint32_t id) : Id(id) {}
			Variable(uint32_t id, Expression *e) : Id(id), Value(e) {}
			~Variable() { clear(); }

			Variable(Variable const &) = delete; //default;
			Variable &operator=(Variable const &rhs) = delete;

			Variable &operator=(Variable &&rhs) noexcept
			{
				if(this != &rhs)
				{
					clear();
					Id = rhs.Id;
					Value = rhs.Value;
					Items = std::move(rhs.Items);
					// clear old item
					rhs.Id = 0;
					rhs.Value = nullptr;
				}
				return *this;
			}

			Variable *find(uint32_t id)
			{
				for(auto &item : Items)
					if(id == item.Id)
						return &item;
				return nullptr;
			}

			void add(uint32_t id, Expression *value)
			{
				if(auto item = find(id))
				{
					delete item->Value;
					item->Value = value;
				}
				else
				{
					Items.emplace_back(id, value);
				}
			}

			void add(Variable &&item)
			{
				if(auto def = find(item.Id))
				{
					for(auto &x : item.Items)
						def->add(std::move(x));

					delete def->Value;
					def->Value = item.Value;
					item.Value = nullptr;
				}
				else
				{
					Items.push_back(std::move(item));
				}
			}

			Expression *at(uint32_t id) const
			{
				for(auto &d : Items)
					if(id == d.Id) return d.Value;
				return nullptr;
			}

			Variable *get(const Ident &id)
			{
				Variable *def = nullptr;
				for(uint32_t i = 0; i < id.length(); i++)
				{
					def = def->find(id[i]);
					if(!def) break;
				}
				return def;
			}

			Variable &&move() noexcept { return std::move(*this); }

			bool empty() const { return Items.empty(); }
			size_t size() const { return Items.size(); }

			auto begin() const { return Items.begin(); }
			auto end() const { return Items.end(); }

			auto begin() { return Items.begin(); }
			auto end() { return Items.end(); }

			void clear()
			{
				Id = 0;
				delete std::exchange(Value, nullptr);
				Items.clear();
			}
		};*/
	}
}