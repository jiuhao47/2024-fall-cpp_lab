// Generated from SafeCLexer.g4 by ANTLR 4.13.2
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue", "this-escape"})
public class SafeCLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.13.2", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		Comma=1, SemiColon=2, Assign=3, LeftBracket=4, RightBracket=5, LeftBrace=6, 
		RightBrace=7, LeftParen=8, RightParen=9, If=10, Else=11, While=12, Equal=13, 
		NonEqual=14, Less=15, Greater=16, LessEqual=17, GreaterEqual=18, Plus=19, 
		Minus=20, Multiply=21, Divide=22, Modulo=23, Int=24, Void=25, Obc=26, 
		Const=27, Identifier=28, IntConst=29, BlockComment=30, LineComment=31, 
		WhiteSpace=32;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"Comma", "SemiColon", "Assign", "LeftBracket", "RightBracket", "LeftBrace", 
			"RightBrace", "LeftParen", "RightParen", "If", "Else", "While", "Equal", 
			"NonEqual", "Less", "Greater", "LessEqual", "GreaterEqual", "Plus", "Minus", 
			"Multiply", "Divide", "Modulo", "Int", "Void", "Obc", "Const", "Identifier", 
			"IntConst", "BlockComment", "LineComment", "WhiteSpace"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "','", "';'", "'='", "'['", "']'", "'{'", "'}'", "'('", "')'", 
			"'if'", "'else'", "'while'", "'=='", "'!='", "'<'", "'>'", "'<='", "'>='", 
			"'+'", "'-'", "'*'", "'/'", "'%'", "'int'", "'void'", "'obc'", "'const'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, "Comma", "SemiColon", "Assign", "LeftBracket", "RightBracket", 
			"LeftBrace", "RightBrace", "LeftParen", "RightParen", "If", "Else", "While", 
			"Equal", "NonEqual", "Less", "Greater", "LessEqual", "GreaterEqual", 
			"Plus", "Minus", "Multiply", "Divide", "Modulo", "Int", "Void", "Obc", 
			"Const", "Identifier", "IntConst", "BlockComment", "LineComment", "WhiteSpace"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}


	public SafeCLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "SafeCLexer.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\u0004\u0000 \u00ba\u0006\uffff\uffff\u0002\u0000\u0007\u0000\u0002\u0001"+
		"\u0007\u0001\u0002\u0002\u0007\u0002\u0002\u0003\u0007\u0003\u0002\u0004"+
		"\u0007\u0004\u0002\u0005\u0007\u0005\u0002\u0006\u0007\u0006\u0002\u0007"+
		"\u0007\u0007\u0002\b\u0007\b\u0002\t\u0007\t\u0002\n\u0007\n\u0002\u000b"+
		"\u0007\u000b\u0002\f\u0007\f\u0002\r\u0007\r\u0002\u000e\u0007\u000e\u0002"+
		"\u000f\u0007\u000f\u0002\u0010\u0007\u0010\u0002\u0011\u0007\u0011\u0002"+
		"\u0012\u0007\u0012\u0002\u0013\u0007\u0013\u0002\u0014\u0007\u0014\u0002"+
		"\u0015\u0007\u0015\u0002\u0016\u0007\u0016\u0002\u0017\u0007\u0017\u0002"+
		"\u0018\u0007\u0018\u0002\u0019\u0007\u0019\u0002\u001a\u0007\u001a\u0002"+
		"\u001b\u0007\u001b\u0002\u001c\u0007\u001c\u0002\u001d\u0007\u001d\u0002"+
		"\u001e\u0007\u001e\u0002\u001f\u0007\u001f\u0001\u0000\u0001\u0000\u0001"+
		"\u0001\u0001\u0001\u0001\u0002\u0001\u0002\u0001\u0003\u0001\u0003\u0001"+
		"\u0004\u0001\u0004\u0001\u0005\u0001\u0005\u0001\u0006\u0001\u0006\u0001"+
		"\u0007\u0001\u0007\u0001\b\u0001\b\u0001\t\u0001\t\u0001\t\u0001\n\u0001"+
		"\n\u0001\n\u0001\n\u0001\n\u0001\u000b\u0001\u000b\u0001\u000b\u0001\u000b"+
		"\u0001\u000b\u0001\u000b\u0001\f\u0001\f\u0001\f\u0001\r\u0001\r\u0001"+
		"\r\u0001\u000e\u0001\u000e\u0001\u000f\u0001\u000f\u0001\u0010\u0001\u0010"+
		"\u0001\u0010\u0001\u0011\u0001\u0011\u0001\u0011\u0001\u0012\u0001\u0012"+
		"\u0001\u0013\u0001\u0013\u0001\u0014\u0001\u0014\u0001\u0015\u0001\u0015"+
		"\u0001\u0016\u0001\u0016\u0001\u0017\u0001\u0017\u0001\u0017\u0001\u0017"+
		"\u0001\u0018\u0001\u0018\u0001\u0018\u0001\u0018\u0001\u0018\u0001\u0019"+
		"\u0001\u0019\u0001\u0019\u0001\u0019\u0001\u001a\u0001\u001a\u0001\u001a"+
		"\u0001\u001a\u0001\u001a\u0001\u001a\u0001\u001b\u0001\u001b\u0005\u001b"+
		"\u0091\b\u001b\n\u001b\f\u001b\u0094\t\u001b\u0001\u001c\u0004\u001c\u0097"+
		"\b\u001c\u000b\u001c\f\u001c\u0098\u0001\u001d\u0001\u001d\u0001\u001d"+
		"\u0001\u001d\u0005\u001d\u009f\b\u001d\n\u001d\f\u001d\u00a2\t\u001d\u0001"+
		"\u001d\u0001\u001d\u0001\u001d\u0001\u001d\u0001\u001d\u0001\u001e\u0001"+
		"\u001e\u0001\u001e\u0001\u001e\u0005\u001e\u00ad\b\u001e\n\u001e\f\u001e"+
		"\u00b0\t\u001e\u0001\u001e\u0001\u001e\u0001\u001f\u0004\u001f\u00b5\b"+
		"\u001f\u000b\u001f\f\u001f\u00b6\u0001\u001f\u0001\u001f\u0001\u00a0\u0000"+
		" \u0001\u0001\u0003\u0002\u0005\u0003\u0007\u0004\t\u0005\u000b\u0006"+
		"\r\u0007\u000f\b\u0011\t\u0013\n\u0015\u000b\u0017\f\u0019\r\u001b\u000e"+
		"\u001d\u000f\u001f\u0010!\u0011#\u0012%\u0013\'\u0014)\u0015+\u0016-\u0017"+
		"/\u00181\u00193\u001a5\u001b7\u001c9\u001d;\u001e=\u001f? \u0001\u0000"+
		"\u0005\u0003\u0000AZ__az\u0004\u000009AZ__az\u0001\u000009\u0002\u0000"+
		"\n\n\r\r\u0003\u0000\t\n\r\r  \u00be\u0000\u0001\u0001\u0000\u0000\u0000"+
		"\u0000\u0003\u0001\u0000\u0000\u0000\u0000\u0005\u0001\u0000\u0000\u0000"+
		"\u0000\u0007\u0001\u0000\u0000\u0000\u0000\t\u0001\u0000\u0000\u0000\u0000"+
		"\u000b\u0001\u0000\u0000\u0000\u0000\r\u0001\u0000\u0000\u0000\u0000\u000f"+
		"\u0001\u0000\u0000\u0000\u0000\u0011\u0001\u0000\u0000\u0000\u0000\u0013"+
		"\u0001\u0000\u0000\u0000\u0000\u0015\u0001\u0000\u0000\u0000\u0000\u0017"+
		"\u0001\u0000\u0000\u0000\u0000\u0019\u0001\u0000\u0000\u0000\u0000\u001b"+
		"\u0001\u0000\u0000\u0000\u0000\u001d\u0001\u0000\u0000\u0000\u0000\u001f"+
		"\u0001\u0000\u0000\u0000\u0000!\u0001\u0000\u0000\u0000\u0000#\u0001\u0000"+
		"\u0000\u0000\u0000%\u0001\u0000\u0000\u0000\u0000\'\u0001\u0000\u0000"+
		"\u0000\u0000)\u0001\u0000\u0000\u0000\u0000+\u0001\u0000\u0000\u0000\u0000"+
		"-\u0001\u0000\u0000\u0000\u0000/\u0001\u0000\u0000\u0000\u00001\u0001"+
		"\u0000\u0000\u0000\u00003\u0001\u0000\u0000\u0000\u00005\u0001\u0000\u0000"+
		"\u0000\u00007\u0001\u0000\u0000\u0000\u00009\u0001\u0000\u0000\u0000\u0000"+
		";\u0001\u0000\u0000\u0000\u0000=\u0001\u0000\u0000\u0000\u0000?\u0001"+
		"\u0000\u0000\u0000\u0001A\u0001\u0000\u0000\u0000\u0003C\u0001\u0000\u0000"+
		"\u0000\u0005E\u0001\u0000\u0000\u0000\u0007G\u0001\u0000\u0000\u0000\t"+
		"I\u0001\u0000\u0000\u0000\u000bK\u0001\u0000\u0000\u0000\rM\u0001\u0000"+
		"\u0000\u0000\u000fO\u0001\u0000\u0000\u0000\u0011Q\u0001\u0000\u0000\u0000"+
		"\u0013S\u0001\u0000\u0000\u0000\u0015V\u0001\u0000\u0000\u0000\u0017["+
		"\u0001\u0000\u0000\u0000\u0019a\u0001\u0000\u0000\u0000\u001bd\u0001\u0000"+
		"\u0000\u0000\u001dg\u0001\u0000\u0000\u0000\u001fi\u0001\u0000\u0000\u0000"+
		"!k\u0001\u0000\u0000\u0000#n\u0001\u0000\u0000\u0000%q\u0001\u0000\u0000"+
		"\u0000\'s\u0001\u0000\u0000\u0000)u\u0001\u0000\u0000\u0000+w\u0001\u0000"+
		"\u0000\u0000-y\u0001\u0000\u0000\u0000/{\u0001\u0000\u0000\u00001\u007f"+
		"\u0001\u0000\u0000\u00003\u0084\u0001\u0000\u0000\u00005\u0088\u0001\u0000"+
		"\u0000\u00007\u008e\u0001\u0000\u0000\u00009\u0096\u0001\u0000\u0000\u0000"+
		";\u009a\u0001\u0000\u0000\u0000=\u00a8\u0001\u0000\u0000\u0000?\u00b4"+
		"\u0001\u0000\u0000\u0000AB\u0005,\u0000\u0000B\u0002\u0001\u0000\u0000"+
		"\u0000CD\u0005;\u0000\u0000D\u0004\u0001\u0000\u0000\u0000EF\u0005=\u0000"+
		"\u0000F\u0006\u0001\u0000\u0000\u0000GH\u0005[\u0000\u0000H\b\u0001\u0000"+
		"\u0000\u0000IJ\u0005]\u0000\u0000J\n\u0001\u0000\u0000\u0000KL\u0005{"+
		"\u0000\u0000L\f\u0001\u0000\u0000\u0000MN\u0005}\u0000\u0000N\u000e\u0001"+
		"\u0000\u0000\u0000OP\u0005(\u0000\u0000P\u0010\u0001\u0000\u0000\u0000"+
		"QR\u0005)\u0000\u0000R\u0012\u0001\u0000\u0000\u0000ST\u0005i\u0000\u0000"+
		"TU\u0005f\u0000\u0000U\u0014\u0001\u0000\u0000\u0000VW\u0005e\u0000\u0000"+
		"WX\u0005l\u0000\u0000XY\u0005s\u0000\u0000YZ\u0005e\u0000\u0000Z\u0016"+
		"\u0001\u0000\u0000\u0000[\\\u0005w\u0000\u0000\\]\u0005h\u0000\u0000]"+
		"^\u0005i\u0000\u0000^_\u0005l\u0000\u0000_`\u0005e\u0000\u0000`\u0018"+
		"\u0001\u0000\u0000\u0000ab\u0005=\u0000\u0000bc\u0005=\u0000\u0000c\u001a"+
		"\u0001\u0000\u0000\u0000de\u0005!\u0000\u0000ef\u0005=\u0000\u0000f\u001c"+
		"\u0001\u0000\u0000\u0000gh\u0005<\u0000\u0000h\u001e\u0001\u0000\u0000"+
		"\u0000ij\u0005>\u0000\u0000j \u0001\u0000\u0000\u0000kl\u0005<\u0000\u0000"+
		"lm\u0005=\u0000\u0000m\"\u0001\u0000\u0000\u0000no\u0005>\u0000\u0000"+
		"op\u0005=\u0000\u0000p$\u0001\u0000\u0000\u0000qr\u0005+\u0000\u0000r"+
		"&\u0001\u0000\u0000\u0000st\u0005-\u0000\u0000t(\u0001\u0000\u0000\u0000"+
		"uv\u0005*\u0000\u0000v*\u0001\u0000\u0000\u0000wx\u0005/\u0000\u0000x"+
		",\u0001\u0000\u0000\u0000yz\u0005%\u0000\u0000z.\u0001\u0000\u0000\u0000"+
		"{|\u0005i\u0000\u0000|}\u0005n\u0000\u0000}~\u0005t\u0000\u0000~0\u0001"+
		"\u0000\u0000\u0000\u007f\u0080\u0005v\u0000\u0000\u0080\u0081\u0005o\u0000"+
		"\u0000\u0081\u0082\u0005i\u0000\u0000\u0082\u0083\u0005d\u0000\u0000\u0083"+
		"2\u0001\u0000\u0000\u0000\u0084\u0085\u0005o\u0000\u0000\u0085\u0086\u0005"+
		"b\u0000\u0000\u0086\u0087\u0005c\u0000\u0000\u00874\u0001\u0000\u0000"+
		"\u0000\u0088\u0089\u0005c\u0000\u0000\u0089\u008a\u0005o\u0000\u0000\u008a"+
		"\u008b\u0005n\u0000\u0000\u008b\u008c\u0005s\u0000\u0000\u008c\u008d\u0005"+
		"t\u0000\u0000\u008d6\u0001\u0000\u0000\u0000\u008e\u0092\u0007\u0000\u0000"+
		"\u0000\u008f\u0091\u0007\u0001\u0000\u0000\u0090\u008f\u0001\u0000\u0000"+
		"\u0000\u0091\u0094\u0001\u0000\u0000\u0000\u0092\u0090\u0001\u0000\u0000"+
		"\u0000\u0092\u0093\u0001\u0000\u0000\u0000\u00938\u0001\u0000\u0000\u0000"+
		"\u0094\u0092\u0001\u0000\u0000\u0000\u0095\u0097\u0007\u0002\u0000\u0000"+
		"\u0096\u0095\u0001\u0000\u0000\u0000\u0097\u0098\u0001\u0000\u0000\u0000"+
		"\u0098\u0096\u0001\u0000\u0000\u0000\u0098\u0099\u0001\u0000\u0000\u0000"+
		"\u0099:\u0001\u0000\u0000\u0000\u009a\u009b\u0005/\u0000\u0000\u009b\u009c"+
		"\u0005*\u0000\u0000\u009c\u00a0\u0001\u0000\u0000\u0000\u009d\u009f\t"+
		"\u0000\u0000\u0000\u009e\u009d\u0001\u0000\u0000\u0000\u009f\u00a2\u0001"+
		"\u0000\u0000\u0000\u00a0\u00a1\u0001\u0000\u0000\u0000\u00a0\u009e\u0001"+
		"\u0000\u0000\u0000\u00a1\u00a3\u0001\u0000\u0000\u0000\u00a2\u00a0\u0001"+
		"\u0000\u0000\u0000\u00a3\u00a4\u0005*\u0000\u0000\u00a4\u00a5\u0005/\u0000"+
		"\u0000\u00a5\u00a6\u0001\u0000\u0000\u0000\u00a6\u00a7\u0006\u001d\u0000"+
		"\u0000\u00a7<\u0001\u0000\u0000\u0000\u00a8\u00a9\u0005/\u0000\u0000\u00a9"+
		"\u00aa\u0005/\u0000\u0000\u00aa\u00ae\u0001\u0000\u0000\u0000\u00ab\u00ad"+
		"\b\u0003\u0000\u0000\u00ac\u00ab\u0001\u0000\u0000\u0000\u00ad\u00b0\u0001"+
		"\u0000\u0000\u0000\u00ae\u00ac\u0001\u0000\u0000\u0000\u00ae\u00af\u0001"+
		"\u0000\u0000\u0000\u00af\u00b1\u0001\u0000\u0000\u0000\u00b0\u00ae\u0001"+
		"\u0000\u0000\u0000\u00b1\u00b2\u0006\u001e\u0000\u0000\u00b2>\u0001\u0000"+
		"\u0000\u0000\u00b3\u00b5\u0007\u0004\u0000\u0000\u00b4\u00b3\u0001\u0000"+
		"\u0000\u0000\u00b5\u00b6\u0001\u0000\u0000\u0000\u00b6\u00b4\u0001\u0000"+
		"\u0000\u0000\u00b6\u00b7\u0001\u0000\u0000\u0000\u00b7\u00b8\u0001\u0000"+
		"\u0000\u0000\u00b8\u00b9\u0006\u001f\u0000\u0000\u00b9@\u0001\u0000\u0000"+
		"\u0000\u0006\u0000\u0092\u0098\u00a0\u00ae\u00b6\u0001\u0006\u0000\u0000";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}