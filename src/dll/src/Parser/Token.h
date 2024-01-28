#pragma once
namespace Nilesoft
{
	namespace Shell
	{

		enum class TokenId
		{
			Error = -2,
			Eof,
			None,
			Skip,
			Func,
			Variable,
			SysVariable,
			Identifier,
			IdentNoArgs,
			Const,
			Number,
			StringL,			// ""
			VString,		// @""

			Comma,			// ,
			OpenParen,		// (
			CloseParen,		// )

			OpenCurly,		// {
			CloseCurly,		// }
			OpenBracket,		// [
			CloseBracket,	// ]
			Question,		// ?
			Dot,			// .
			Quote,			// .
			SingleQuote,			// .

			SlComment,		// '//comment'
			MlComment,		// '/*comment*/'
			Comment,		// slcomment or mlcomment

			Semicolon,		// ;
			Colon,			// :

			Plus,			// +
			Minus,			// -
			Mult,			// *
			Slash,			// /
			Mod,			// %
			Greater,		// >
			Less,			// <
			AmpersAnd,		// &
			LogOr,			// |
			Xor,			// ^
			Bang,			// !
			Equal,			// =

			Star,			// *
			Percent,		// %
			Tilde,			// ~
			Not,			// !
			NotEqual,		// !=
			EqualEqual,		// ==
			PlusEqual,		// +=
			PlusPlus,		// --
			MinusEqual,		// -=
			HateEual,		// ^=
			SlashEqual,		// /=
			AndeEual,		// &=
			LogAnd,			// &&
			BareEual,		// |=
			ShiftLeftEq,	// <<=
			ShiftRightEq,	// >>=
			ShiftLeft,		// <<
			ShiftRight,		// >>
			LessEqual,		// <=
			GreaterEqual,	// >=
			MinusMinus,		// --
			ModEqual,		// %=
			Arrow,			// ->

			If,
			Else,
			Foreach,
			For,
			While,
			Continue,
			PrintComment,

			Shell,
			Variables,
			Static,
			StaticMove,
			StaticRemove,
			StaticRename,
			Menu,
			MenuPopup,
			MenuItem,
			MenuSeprator
		};

		enum class TokenType
		{
			None,
			Skip,
			Numeric,
			String,
			Identifier,
			Variable,
			Operator
		};

		//ErrorCode
		enum class TokenError : unsigned
		{
			None,
			Eof,
			Unknown,
			Invalid,
			InvalidPath,
			InvalidFile,
			CannotFoundFile,

			InvalidConfigPath,
			InvalidConfigFile,
			CannotFoundConfigFile,
			InvalidDefinitionPath,
			InvalidDefinitionFile,
			UnknownFileUnicode,
			FileUnicodeNotSupport,
			FileUnicodeUTF32NotSupport,
			InvalidConfigFileVersion,
			FolderCannotBeFound,
			PathNotFound,
			FailOpenFile,
			FileCannotBeRead,
			ImportPathExpected,
			ImportUnexpected,
			DataCorrupted,

			True,
			False,
			Syntax,
			Expression,
			Numeric,
			HexaDecimalExpected,
			CharacterHexaDecimalExpected,
			IllegalEscape,
			IllegalCharacter,
			IllegalCharacterHexaDecimal,
			ColorInvalid,
			String,
			Operator,

			Expected,
			Unexpected,

			CharacterUnexpected,

			OperatorUnexpected,
			OperatorExpected,

			NumericUnexpected,
			NumericExpected,

			StringUnexpected,
			StringExpected,

			ExpressionExpected,
			ExpressionUnexpected,

			UnaryExpressionExpected,
			UnaryExpressionUnexpected,

			BinaryExpressionExpected,
			BinaryExpressionUnexpected,

			TernaryExpressionExpected,
			TernarySemicolonExpected,

			Identifier,
			IdentifierExpected,
			IdentifierUnexpected,
			IdentifierUndefined,
			IdentifierMaxLength,
			IdentifierArguments,
			IdentifierArgumentsUnexpected,
			IdentifierArgumentsExpected,

			IdentifierConfig,
			IdentifierConfigExpected,
			IdentifierConfigUnexpected,

			CloseCommentExpected,
			OpenEndedValue,

			//InvalidNumber,
			//NumericIntOverflow,
			StringTerminatedExpected,
			StringUnterminated,
			StringCharacterEscapeSequence,
			NewlineInStringUnexpected,

			MenuTypeExpected,
			ShellExpected,
			DynamicExpected,

			PropertyUndefined,
			PropertyExpected,
			PropertyUnexpected,
			PropertyTypeUnexpected,
			PropertyType,
			PropertyDuplicate,
			PropertyValueUnexpected,
			PropertyValueEmptyUnexpected,

			PropertyName,
			PropertyValue,
			PropertyValueExpected,
			PropertyValueUnterminated,

			PropertyTitleOrImageExpected,
			PropertyCommandAndArgumentsCannotCombine,
			PropertyTypeAndSubTypeCannotCombine,
			PropertyNoneExcludeValueUnexpected,
			PropertyFindOrWhereExpected,
			
			PropertyCommandExpected,

			OpenCurlyExpected,		// {
			CloseCurlyExpected,		// }

			OpenCurlyUnexpected,
			CloseCurlyUnexpected,

			OpenParenExpected,		// (
			CloseParenExpected,		// )

			OpenBracketExpected,		// [
			CloseBracketExpected,	// ]

			EOFUnexpected,

			QuoteExpected,
			SingleQuoteExpected,
			DoubleQuoteExpected,

			MissingQuote,
			OpenQuoteExpected,
			OpenSingleQuoteExpected,
			OpenDoubleQuoteExpected,
			CloseQuoteExpected,
			CloseSingleQuoteExpected,
			CloseDoubleQuoteExpected,
			QuoteOrExprestionExpected,

			VariableExpected,
			VariableUnexpected,

			AssignExpected,
			AssignUnexpected,
			AssignLeftUnexpected,
			AssignRightUnexpected,
			AssignOrOpenBracketExpected,

			ColonExpected,
			SemicolonExpected,
			CommaExpected,
			CommaOrClosParenExpected,
			StringOrNumericExpected,
			CommentSinglelineUnexpected,

			CacheOverflow,
			CannotOpenWriteFile,				//unable to open file for writing
			CannotWriteFile						//cannot write into file
		};
	}
}

