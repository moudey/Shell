
using System;

namespace Nilesoft
{
	public static class Hash
	{
		public const ulong offset_basis_64 = 14695981039346656037U;
		public const ulong prime_64 = 1099511628211UL;
		public const uint offset_basis = 2166136261U;
		public const uint prime = 16777619U;

		//constexpr auto
		//const uint offset_basis = 5381U;
		//const auto prime = 33U;

		public static ulong hash64(string str)
		{
			var h = offset_basis_64;
			foreach (var c in str)
			{
				h ^= c;
				h *= prime_64;
			}
			return h;
		}

		public static uint hash(string str)
		{
			var h = offset_basis;
			foreach (var c in str) {
				h ^= c;
				h *= prime;
			}
			return h;
		}

		public const int Magic = 5381;

		public static int Generate(int hash, char value)
		{
			return hash * 33 + char.ToLower(value);
		}

		public static int Generate(string value)
		{
			string text = value.Trim();
			int num = 5381;
			int num3;
			for (int i = 0; i < text.Length; i = num3 + 1)
			{
				num = Generate(num, text[i]);
				num3 = i;
			}
			return (num != 5381) ? num : 0;
		}

		public static string ToString(string input)
		{
			if (string.IsNullOrEmpty(input))
			{
				return null;
			}
			return "0x" + $"{Generate(input):X8}U";
		}
	}
}