// Generated from SafeCParser.g4 by ANTLR 4.13.2
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue", "this-escape"})
public class SafeCParser extends Parser {
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
	public static final int
		RULE_compUnit = 0, RULE_decl = 1, RULE_funcDef = 2, RULE_constDecl = 3, 
		RULE_constDef = 4, RULE_varDecl = 5, RULE_bType = 6, RULE_varDef = 7, 
		RULE_array = 8, RULE_obcArray = 9, RULE_unobcArray = 10, RULE_block = 11, 
		RULE_blockItem = 12, RULE_stmt = 13, RULE_cond = 14, RULE_lval = 15, RULE_number = 16, 
		RULE_exp = 17;
	private static String[] makeRuleNames() {
		return new String[] {
			"compUnit", "decl", "funcDef", "constDecl", "constDef", "varDecl", "bType", 
			"varDef", "array", "obcArray", "unobcArray", "block", "blockItem", "stmt", 
			"cond", "lval", "number", "exp"
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

	@Override
	public String getGrammarFileName() { return "SafeCParser.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public SafeCParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@SuppressWarnings("CheckReturnValue")
	public static class CompUnitContext extends ParserRuleContext {
		public TerminalNode EOF() { return getToken(SafeCParser.EOF, 0); }
		public List<DeclContext> decl() {
			return getRuleContexts(DeclContext.class);
		}
		public DeclContext decl(int i) {
			return getRuleContext(DeclContext.class,i);
		}
		public List<FuncDefContext> funcDef() {
			return getRuleContexts(FuncDefContext.class);
		}
		public FuncDefContext funcDef(int i) {
			return getRuleContext(FuncDefContext.class,i);
		}
		public CompUnitContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_compUnit; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterCompUnit(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitCompUnit(this);
		}
	}

	public final CompUnitContext compUnit() throws RecognitionException {
		CompUnitContext _localctx = new CompUnitContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_compUnit);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(38); 
			_errHandler.sync(this);
			_la = _input.LA(1);
			do {
				{
				setState(38);
				_errHandler.sync(this);
				switch (_input.LA(1)) {
				case Int:
				case Const:
					{
					setState(36);
					decl();
					}
					break;
				case Void:
					{
					setState(37);
					funcDef();
					}
					break;
				default:
					throw new NoViableAltException(this);
				}
				}
				setState(40); 
				_errHandler.sync(this);
				_la = _input.LA(1);
			} while ( (((_la) & ~0x3f) == 0 && ((1L << _la) & 184549376L) != 0) );
			setState(42);
			match(EOF);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class DeclContext extends ParserRuleContext {
		public ConstDeclContext constDecl() {
			return getRuleContext(ConstDeclContext.class,0);
		}
		public VarDeclContext varDecl() {
			return getRuleContext(VarDeclContext.class,0);
		}
		public DeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_decl; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterDecl(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitDecl(this);
		}
	}

	public final DeclContext decl() throws RecognitionException {
		DeclContext _localctx = new DeclContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_decl);
		try {
			setState(46);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case Const:
				enterOuterAlt(_localctx, 1);
				{
				setState(44);
				constDecl();
				}
				break;
			case Int:
				enterOuterAlt(_localctx, 2);
				{
				setState(45);
				varDecl();
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class FuncDefContext extends ParserRuleContext {
		public TerminalNode Void() { return getToken(SafeCParser.Void, 0); }
		public TerminalNode Identifier() { return getToken(SafeCParser.Identifier, 0); }
		public TerminalNode LeftParen() { return getToken(SafeCParser.LeftParen, 0); }
		public TerminalNode RightParen() { return getToken(SafeCParser.RightParen, 0); }
		public BlockContext block() {
			return getRuleContext(BlockContext.class,0);
		}
		public FuncDefContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_funcDef; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterFuncDef(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitFuncDef(this);
		}
	}

	public final FuncDefContext funcDef() throws RecognitionException {
		FuncDefContext _localctx = new FuncDefContext(_ctx, getState());
		enterRule(_localctx, 4, RULE_funcDef);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(48);
			match(Void);
			setState(49);
			match(Identifier);
			setState(50);
			match(LeftParen);
			setState(51);
			match(RightParen);
			setState(52);
			block();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ConstDeclContext extends ParserRuleContext {
		public TerminalNode Const() { return getToken(SafeCParser.Const, 0); }
		public BTypeContext bType() {
			return getRuleContext(BTypeContext.class,0);
		}
		public List<ConstDefContext> constDef() {
			return getRuleContexts(ConstDefContext.class);
		}
		public ConstDefContext constDef(int i) {
			return getRuleContext(ConstDefContext.class,i);
		}
		public TerminalNode SemiColon() { return getToken(SafeCParser.SemiColon, 0); }
		public List<TerminalNode> Comma() { return getTokens(SafeCParser.Comma); }
		public TerminalNode Comma(int i) {
			return getToken(SafeCParser.Comma, i);
		}
		public ConstDeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_constDecl; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterConstDecl(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitConstDecl(this);
		}
	}

	public final ConstDeclContext constDecl() throws RecognitionException {
		ConstDeclContext _localctx = new ConstDeclContext(_ctx, getState());
		enterRule(_localctx, 6, RULE_constDecl);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(54);
			match(Const);
			setState(55);
			bType();
			setState(56);
			constDef();
			setState(61);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==Comma) {
				{
				{
				setState(57);
				match(Comma);
				setState(58);
				constDef();
				}
				}
				setState(63);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(64);
			match(SemiColon);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ConstDefContext extends ParserRuleContext {
		public TerminalNode Assign() { return getToken(SafeCParser.Assign, 0); }
		public TerminalNode Identifier() { return getToken(SafeCParser.Identifier, 0); }
		public ArrayContext array() {
			return getRuleContext(ArrayContext.class,0);
		}
		public List<ExpContext> exp() {
			return getRuleContexts(ExpContext.class);
		}
		public ExpContext exp(int i) {
			return getRuleContext(ExpContext.class,i);
		}
		public TerminalNode LeftBrace() { return getToken(SafeCParser.LeftBrace, 0); }
		public TerminalNode RightBrace() { return getToken(SafeCParser.RightBrace, 0); }
		public List<TerminalNode> Comma() { return getTokens(SafeCParser.Comma); }
		public TerminalNode Comma(int i) {
			return getToken(SafeCParser.Comma, i);
		}
		public ConstDefContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_constDef; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterConstDef(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitConstDef(this);
		}
	}

	public final ConstDefContext constDef() throws RecognitionException {
		ConstDefContext _localctx = new ConstDefContext(_ctx, getState());
		enterRule(_localctx, 8, RULE_constDef);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(68);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,4,_ctx) ) {
			case 1:
				{
				setState(66);
				match(Identifier);
				}
				break;
			case 2:
				{
				setState(67);
				array();
				}
				break;
			}
			setState(70);
			match(Assign);
			setState(84);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case LeftParen:
			case Plus:
			case Minus:
			case Identifier:
			case IntConst:
				{
				setState(71);
				exp(0);
				}
				break;
			case LeftBrace:
				{
				{
				setState(72);
				match(LeftBrace);
				setState(81);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & 806879488L) != 0)) {
					{
					setState(73);
					exp(0);
					setState(78);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==Comma) {
						{
						{
						setState(74);
						match(Comma);
						setState(75);
						exp(0);
						}
						}
						setState(80);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					}
				}

				setState(83);
				match(RightBrace);
				}
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class VarDeclContext extends ParserRuleContext {
		public BTypeContext bType() {
			return getRuleContext(BTypeContext.class,0);
		}
		public List<VarDefContext> varDef() {
			return getRuleContexts(VarDefContext.class);
		}
		public VarDefContext varDef(int i) {
			return getRuleContext(VarDefContext.class,i);
		}
		public TerminalNode SemiColon() { return getToken(SafeCParser.SemiColon, 0); }
		public List<TerminalNode> Comma() { return getTokens(SafeCParser.Comma); }
		public TerminalNode Comma(int i) {
			return getToken(SafeCParser.Comma, i);
		}
		public VarDeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_varDecl; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterVarDecl(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitVarDecl(this);
		}
	}

	public final VarDeclContext varDecl() throws RecognitionException {
		VarDeclContext _localctx = new VarDeclContext(_ctx, getState());
		enterRule(_localctx, 10, RULE_varDecl);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(86);
			bType();
			setState(87);
			varDef();
			setState(92);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==Comma) {
				{
				{
				setState(88);
				match(Comma);
				setState(89);
				varDef();
				}
				}
				setState(94);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(95);
			match(SemiColon);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class BTypeContext extends ParserRuleContext {
		public TerminalNode Int() { return getToken(SafeCParser.Int, 0); }
		public BTypeContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_bType; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterBType(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitBType(this);
		}
	}

	public final BTypeContext bType() throws RecognitionException {
		BTypeContext _localctx = new BTypeContext(_ctx, getState());
		enterRule(_localctx, 12, RULE_bType);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(97);
			match(Int);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class VarDefContext extends ParserRuleContext {
		public TerminalNode Identifier() { return getToken(SafeCParser.Identifier, 0); }
		public ArrayContext array() {
			return getRuleContext(ArrayContext.class,0);
		}
		public TerminalNode Assign() { return getToken(SafeCParser.Assign, 0); }
		public List<ExpContext> exp() {
			return getRuleContexts(ExpContext.class);
		}
		public ExpContext exp(int i) {
			return getRuleContext(ExpContext.class,i);
		}
		public TerminalNode LeftBrace() { return getToken(SafeCParser.LeftBrace, 0); }
		public TerminalNode RightBrace() { return getToken(SafeCParser.RightBrace, 0); }
		public List<TerminalNode> Comma() { return getTokens(SafeCParser.Comma); }
		public TerminalNode Comma(int i) {
			return getToken(SafeCParser.Comma, i);
		}
		public VarDefContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_varDef; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterVarDef(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitVarDef(this);
		}
	}

	public final VarDefContext varDef() throws RecognitionException {
		VarDefContext _localctx = new VarDefContext(_ctx, getState());
		enterRule(_localctx, 14, RULE_varDef);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(101);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,9,_ctx) ) {
			case 1:
				{
				setState(99);
				match(Identifier);
				}
				break;
			case 2:
				{
				setState(100);
				array();
				}
				break;
			}
			setState(119);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==Assign) {
				{
				setState(103);
				match(Assign);
				setState(117);
				_errHandler.sync(this);
				switch (_input.LA(1)) {
				case LeftParen:
				case Plus:
				case Minus:
				case Identifier:
				case IntConst:
					{
					setState(104);
					exp(0);
					}
					break;
				case LeftBrace:
					{
					{
					setState(105);
					match(LeftBrace);
					setState(114);
					_errHandler.sync(this);
					_la = _input.LA(1);
					if ((((_la) & ~0x3f) == 0 && ((1L << _la) & 806879488L) != 0)) {
						{
						setState(106);
						exp(0);
						setState(111);
						_errHandler.sync(this);
						_la = _input.LA(1);
						while (_la==Comma) {
							{
							{
							setState(107);
							match(Comma);
							setState(108);
							exp(0);
							}
							}
							setState(113);
							_errHandler.sync(this);
							_la = _input.LA(1);
						}
						}
					}

					setState(116);
					match(RightBrace);
					}
					}
					break;
				default:
					throw new NoViableAltException(this);
				}
				}
			}

			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ArrayContext extends ParserRuleContext {
		public ObcArrayContext obcArray() {
			return getRuleContext(ObcArrayContext.class,0);
		}
		public UnobcArrayContext unobcArray() {
			return getRuleContext(UnobcArrayContext.class,0);
		}
		public ArrayContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_array; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterArray(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitArray(this);
		}
	}

	public final ArrayContext array() throws RecognitionException {
		ArrayContext _localctx = new ArrayContext(_ctx, getState());
		enterRule(_localctx, 16, RULE_array);
		try {
			setState(123);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case Obc:
				enterOuterAlt(_localctx, 1);
				{
				setState(121);
				obcArray();
				}
				break;
			case Identifier:
				enterOuterAlt(_localctx, 2);
				{
				setState(122);
				unobcArray();
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ObcArrayContext extends ParserRuleContext {
		public TerminalNode Obc() { return getToken(SafeCParser.Obc, 0); }
		public UnobcArrayContext unobcArray() {
			return getRuleContext(UnobcArrayContext.class,0);
		}
		public ObcArrayContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_obcArray; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterObcArray(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitObcArray(this);
		}
	}

	public final ObcArrayContext obcArray() throws RecognitionException {
		ObcArrayContext _localctx = new ObcArrayContext(_ctx, getState());
		enterRule(_localctx, 18, RULE_obcArray);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(125);
			match(Obc);
			setState(126);
			unobcArray();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class UnobcArrayContext extends ParserRuleContext {
		public TerminalNode Identifier() { return getToken(SafeCParser.Identifier, 0); }
		public TerminalNode LeftBracket() { return getToken(SafeCParser.LeftBracket, 0); }
		public TerminalNode RightBracket() { return getToken(SafeCParser.RightBracket, 0); }
		public ExpContext exp() {
			return getRuleContext(ExpContext.class,0);
		}
		public UnobcArrayContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_unobcArray; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterUnobcArray(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitUnobcArray(this);
		}
	}

	public final UnobcArrayContext unobcArray() throws RecognitionException {
		UnobcArrayContext _localctx = new UnobcArrayContext(_ctx, getState());
		enterRule(_localctx, 20, RULE_unobcArray);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(128);
			match(Identifier);
			setState(129);
			match(LeftBracket);
			setState(131);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if ((((_la) & ~0x3f) == 0 && ((1L << _la) & 806879488L) != 0)) {
				{
				setState(130);
				exp(0);
				}
			}

			setState(133);
			match(RightBracket);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class BlockContext extends ParserRuleContext {
		public TerminalNode LeftBrace() { return getToken(SafeCParser.LeftBrace, 0); }
		public TerminalNode RightBrace() { return getToken(SafeCParser.RightBrace, 0); }
		public List<BlockItemContext> blockItem() {
			return getRuleContexts(BlockItemContext.class);
		}
		public BlockItemContext blockItem(int i) {
			return getRuleContext(BlockItemContext.class,i);
		}
		public BlockContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_block; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterBlock(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitBlock(this);
		}
	}

	public final BlockContext block() throws RecognitionException {
		BlockContext _localctx = new BlockContext(_ctx, getState());
		enterRule(_localctx, 22, RULE_block);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(135);
			match(LeftBrace);
			setState(139);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & 957879620L) != 0)) {
				{
				{
				setState(136);
				blockItem();
				}
				}
				setState(141);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(142);
			match(RightBrace);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class BlockItemContext extends ParserRuleContext {
		public DeclContext decl() {
			return getRuleContext(DeclContext.class,0);
		}
		public StmtContext stmt() {
			return getRuleContext(StmtContext.class,0);
		}
		public BlockItemContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_blockItem; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterBlockItem(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitBlockItem(this);
		}
	}

	public final BlockItemContext blockItem() throws RecognitionException {
		BlockItemContext _localctx = new BlockItemContext(_ctx, getState());
		enterRule(_localctx, 24, RULE_blockItem);
		try {
			setState(146);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case Int:
			case Const:
				enterOuterAlt(_localctx, 1);
				{
				setState(144);
				decl();
				}
				break;
			case SemiColon:
			case LeftBrace:
			case LeftParen:
			case If:
			case While:
			case Plus:
			case Minus:
			case Identifier:
			case IntConst:
				enterOuterAlt(_localctx, 2);
				{
				setState(145);
				stmt();
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class StmtContext extends ParserRuleContext {
		public BlockContext block() {
			return getRuleContext(BlockContext.class,0);
		}
		public TerminalNode SemiColon() { return getToken(SafeCParser.SemiColon, 0); }
		public ExpContext exp() {
			return getRuleContext(ExpContext.class,0);
		}
		public TerminalNode If() { return getToken(SafeCParser.If, 0); }
		public TerminalNode LeftParen() { return getToken(SafeCParser.LeftParen, 0); }
		public CondContext cond() {
			return getRuleContext(CondContext.class,0);
		}
		public TerminalNode RightParen() { return getToken(SafeCParser.RightParen, 0); }
		public List<StmtContext> stmt() {
			return getRuleContexts(StmtContext.class);
		}
		public StmtContext stmt(int i) {
			return getRuleContext(StmtContext.class,i);
		}
		public TerminalNode Else() { return getToken(SafeCParser.Else, 0); }
		public TerminalNode While() { return getToken(SafeCParser.While, 0); }
		public StmtContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_stmt; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterStmt(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitStmt(this);
		}
	}

	public final StmtContext stmt() throws RecognitionException {
		StmtContext _localctx = new StmtContext(_ctx, getState());
		enterRule(_localctx, 26, RULE_stmt);
		try {
			setState(168);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case LeftBrace:
				enterOuterAlt(_localctx, 1);
				{
				setState(148);
				block();
				}
				break;
			case SemiColon:
				enterOuterAlt(_localctx, 2);
				{
				setState(149);
				match(SemiColon);
				}
				break;
			case LeftParen:
			case Plus:
			case Minus:
			case Identifier:
			case IntConst:
				enterOuterAlt(_localctx, 3);
				{
				setState(150);
				exp(0);
				setState(151);
				match(SemiColon);
				}
				break;
			case If:
				enterOuterAlt(_localctx, 4);
				{
				setState(153);
				match(If);
				setState(154);
				match(LeftParen);
				setState(155);
				cond();
				setState(156);
				match(RightParen);
				setState(157);
				stmt();
				setState(160);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,18,_ctx) ) {
				case 1:
					{
					setState(158);
					match(Else);
					setState(159);
					stmt();
					}
					break;
				}
				}
				break;
			case While:
				enterOuterAlt(_localctx, 5);
				{
				setState(162);
				match(While);
				setState(163);
				match(LeftParen);
				setState(164);
				cond();
				setState(165);
				match(RightParen);
				setState(166);
				stmt();
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class CondContext extends ParserRuleContext {
		public ExpContext exp() {
			return getRuleContext(ExpContext.class,0);
		}
		public CondContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_cond; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterCond(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitCond(this);
		}
	}

	public final CondContext cond() throws RecognitionException {
		CondContext _localctx = new CondContext(_ctx, getState());
		enterRule(_localctx, 28, RULE_cond);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(170);
			exp(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class LvalContext extends ParserRuleContext {
		public TerminalNode Identifier() { return getToken(SafeCParser.Identifier, 0); }
		public TerminalNode LeftBracket() { return getToken(SafeCParser.LeftBracket, 0); }
		public ExpContext exp() {
			return getRuleContext(ExpContext.class,0);
		}
		public TerminalNode RightBracket() { return getToken(SafeCParser.RightBracket, 0); }
		public TerminalNode LeftParen() { return getToken(SafeCParser.LeftParen, 0); }
		public TerminalNode RightParen() { return getToken(SafeCParser.RightParen, 0); }
		public LvalContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_lval; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterLval(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitLval(this);
		}
	}

	public final LvalContext lval() throws RecognitionException {
		LvalContext _localctx = new LvalContext(_ctx, getState());
		enterRule(_localctx, 30, RULE_lval);
		try {
			setState(181);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,20,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(172);
				match(Identifier);
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				{
				setState(173);
				match(Identifier);
				setState(174);
				match(LeftBracket);
				setState(175);
				exp(0);
				setState(176);
				match(RightBracket);
				}
				}
				break;
			case 3:
				enterOuterAlt(_localctx, 3);
				{
				{
				setState(178);
				match(Identifier);
				setState(179);
				match(LeftParen);
				setState(180);
				match(RightParen);
				}
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class NumberContext extends ParserRuleContext {
		public TerminalNode IntConst() { return getToken(SafeCParser.IntConst, 0); }
		public NumberContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_number; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterNumber(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitNumber(this);
		}
	}

	public final NumberContext number() throws RecognitionException {
		NumberContext _localctx = new NumberContext(_ctx, getState());
		enterRule(_localctx, 32, RULE_number);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(183);
			match(IntConst);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ExpContext extends ParserRuleContext {
		public LvalContext lval() {
			return getRuleContext(LvalContext.class,0);
		}
		public TerminalNode Assign() { return getToken(SafeCParser.Assign, 0); }
		public List<ExpContext> exp() {
			return getRuleContexts(ExpContext.class);
		}
		public ExpContext exp(int i) {
			return getRuleContext(ExpContext.class,i);
		}
		public NumberContext number() {
			return getRuleContext(NumberContext.class,0);
		}
		public TerminalNode LeftParen() { return getToken(SafeCParser.LeftParen, 0); }
		public TerminalNode RightParen() { return getToken(SafeCParser.RightParen, 0); }
		public TerminalNode Minus() { return getToken(SafeCParser.Minus, 0); }
		public TerminalNode Plus() { return getToken(SafeCParser.Plus, 0); }
		public TerminalNode Multiply() { return getToken(SafeCParser.Multiply, 0); }
		public TerminalNode Divide() { return getToken(SafeCParser.Divide, 0); }
		public TerminalNode Modulo() { return getToken(SafeCParser.Modulo, 0); }
		public TerminalNode Equal() { return getToken(SafeCParser.Equal, 0); }
		public TerminalNode NonEqual() { return getToken(SafeCParser.NonEqual, 0); }
		public TerminalNode Less() { return getToken(SafeCParser.Less, 0); }
		public TerminalNode Greater() { return getToken(SafeCParser.Greater, 0); }
		public TerminalNode LessEqual() { return getToken(SafeCParser.LessEqual, 0); }
		public TerminalNode GreaterEqual() { return getToken(SafeCParser.GreaterEqual, 0); }
		public ExpContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_exp; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).enterExp(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof SafeCParserListener ) ((SafeCParserListener)listener).exitExp(this);
		}
	}

	public final ExpContext exp() throws RecognitionException {
		return exp(0);
	}

	private ExpContext exp(int _p) throws RecognitionException {
		ParserRuleContext _parentctx = _ctx;
		int _parentState = getState();
		ExpContext _localctx = new ExpContext(_ctx, _parentState);
		ExpContext _prevctx = _localctx;
		int _startState = 34;
		enterRecursionRule(_localctx, 34, RULE_exp, _p);
		int _la;
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(198);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,21,_ctx) ) {
			case 1:
				{
				setState(186);
				lval();
				setState(187);
				match(Assign);
				setState(188);
				exp(7);
				}
				break;
			case 2:
				{
				setState(190);
				lval();
				}
				break;
			case 3:
				{
				setState(191);
				number();
				}
				break;
			case 4:
				{
				setState(192);
				match(LeftParen);
				setState(193);
				exp(0);
				setState(194);
				match(RightParen);
				}
				break;
			case 5:
				{
				setState(196);
				_la = _input.LA(1);
				if ( !(_la==Plus || _la==Minus) ) {
				_errHandler.recoverInline(this);
				}
				else {
					if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
					_errHandler.reportMatch(this);
					consume();
				}
				setState(197);
				exp(2);
				}
				break;
			}
			_ctx.stop = _input.LT(-1);
			setState(208);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,23,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					if ( _parseListeners!=null ) triggerExitRuleEvent();
					_prevctx = _localctx;
					{
					setState(206);
					_errHandler.sync(this);
					switch ( getInterpreter().adaptivePredict(_input,22,_ctx) ) {
					case 1:
						{
						_localctx = new ExpContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_exp);
						setState(200);
						if (!(precpred(_ctx, 3))) throw new FailedPredicateException(this, "precpred(_ctx, 3)");
						setState(201);
						_la = _input.LA(1);
						if ( !((((_la) & ~0x3f) == 0 && ((1L << _la) & 16252928L) != 0)) ) {
						_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(202);
						exp(4);
						}
						break;
					case 2:
						{
						_localctx = new ExpContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_exp);
						setState(203);
						if (!(precpred(_ctx, 1))) throw new FailedPredicateException(this, "precpred(_ctx, 1)");
						setState(204);
						_la = _input.LA(1);
						if ( !((((_la) & ~0x3f) == 0 && ((1L << _la) & 516096L) != 0)) ) {
						_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(205);
						exp(2);
						}
						break;
					}
					} 
				}
				setState(210);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,23,_ctx);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			unrollRecursionContexts(_parentctx);
		}
		return _localctx;
	}

	public boolean sempred(RuleContext _localctx, int ruleIndex, int predIndex) {
		switch (ruleIndex) {
		case 17:
			return exp_sempred((ExpContext)_localctx, predIndex);
		}
		return true;
	}
	private boolean exp_sempred(ExpContext _localctx, int predIndex) {
		switch (predIndex) {
		case 0:
			return precpred(_ctx, 3);
		case 1:
			return precpred(_ctx, 1);
		}
		return true;
	}

	public static final String _serializedATN =
		"\u0004\u0001 \u00d4\u0002\u0000\u0007\u0000\u0002\u0001\u0007\u0001\u0002"+
		"\u0002\u0007\u0002\u0002\u0003\u0007\u0003\u0002\u0004\u0007\u0004\u0002"+
		"\u0005\u0007\u0005\u0002\u0006\u0007\u0006\u0002\u0007\u0007\u0007\u0002"+
		"\b\u0007\b\u0002\t\u0007\t\u0002\n\u0007\n\u0002\u000b\u0007\u000b\u0002"+
		"\f\u0007\f\u0002\r\u0007\r\u0002\u000e\u0007\u000e\u0002\u000f\u0007\u000f"+
		"\u0002\u0010\u0007\u0010\u0002\u0011\u0007\u0011\u0001\u0000\u0001\u0000"+
		"\u0004\u0000\'\b\u0000\u000b\u0000\f\u0000(\u0001\u0000\u0001\u0000\u0001"+
		"\u0001\u0001\u0001\u0003\u0001/\b\u0001\u0001\u0002\u0001\u0002\u0001"+
		"\u0002\u0001\u0002\u0001\u0002\u0001\u0002\u0001\u0003\u0001\u0003\u0001"+
		"\u0003\u0001\u0003\u0001\u0003\u0005\u0003<\b\u0003\n\u0003\f\u0003?\t"+
		"\u0003\u0001\u0003\u0001\u0003\u0001\u0004\u0001\u0004\u0003\u0004E\b"+
		"\u0004\u0001\u0004\u0001\u0004\u0001\u0004\u0001\u0004\u0001\u0004\u0001"+
		"\u0004\u0005\u0004M\b\u0004\n\u0004\f\u0004P\t\u0004\u0003\u0004R\b\u0004"+
		"\u0001\u0004\u0003\u0004U\b\u0004\u0001\u0005\u0001\u0005\u0001\u0005"+
		"\u0001\u0005\u0005\u0005[\b\u0005\n\u0005\f\u0005^\t\u0005\u0001\u0005"+
		"\u0001\u0005\u0001\u0006\u0001\u0006\u0001\u0007\u0001\u0007\u0003\u0007"+
		"f\b\u0007\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007"+
		"\u0001\u0007\u0005\u0007n\b\u0007\n\u0007\f\u0007q\t\u0007\u0003\u0007"+
		"s\b\u0007\u0001\u0007\u0003\u0007v\b\u0007\u0003\u0007x\b\u0007\u0001"+
		"\b\u0001\b\u0003\b|\b\b\u0001\t\u0001\t\u0001\t\u0001\n\u0001\n\u0001"+
		"\n\u0003\n\u0084\b\n\u0001\n\u0001\n\u0001\u000b\u0001\u000b\u0005\u000b"+
		"\u008a\b\u000b\n\u000b\f\u000b\u008d\t\u000b\u0001\u000b\u0001\u000b\u0001"+
		"\f\u0001\f\u0003\f\u0093\b\f\u0001\r\u0001\r\u0001\r\u0001\r\u0001\r\u0001"+
		"\r\u0001\r\u0001\r\u0001\r\u0001\r\u0001\r\u0001\r\u0003\r\u00a1\b\r\u0001"+
		"\r\u0001\r\u0001\r\u0001\r\u0001\r\u0001\r\u0003\r\u00a9\b\r\u0001\u000e"+
		"\u0001\u000e\u0001\u000f\u0001\u000f\u0001\u000f\u0001\u000f\u0001\u000f"+
		"\u0001\u000f\u0001\u000f\u0001\u000f\u0001\u000f\u0003\u000f\u00b6\b\u000f"+
		"\u0001\u0010\u0001\u0010\u0001\u0011\u0001\u0011\u0001\u0011\u0001\u0011"+
		"\u0001\u0011\u0001\u0011\u0001\u0011\u0001\u0011\u0001\u0011\u0001\u0011"+
		"\u0001\u0011\u0001\u0011\u0001\u0011\u0003\u0011\u00c7\b\u0011\u0001\u0011"+
		"\u0001\u0011\u0001\u0011\u0001\u0011\u0001\u0011\u0001\u0011\u0005\u0011"+
		"\u00cf\b\u0011\n\u0011\f\u0011\u00d2\t\u0011\u0001\u0011\u0000\u0001\""+
		"\u0012\u0000\u0002\u0004\u0006\b\n\f\u000e\u0010\u0012\u0014\u0016\u0018"+
		"\u001a\u001c\u001e \"\u0000\u0003\u0001\u0000\u0013\u0014\u0001\u0000"+
		"\u0013\u0017\u0001\u0000\r\u0012\u00e0\u0000&\u0001\u0000\u0000\u0000"+
		"\u0002.\u0001\u0000\u0000\u0000\u00040\u0001\u0000\u0000\u0000\u00066"+
		"\u0001\u0000\u0000\u0000\bD\u0001\u0000\u0000\u0000\nV\u0001\u0000\u0000"+
		"\u0000\fa\u0001\u0000\u0000\u0000\u000ee\u0001\u0000\u0000\u0000\u0010"+
		"{\u0001\u0000\u0000\u0000\u0012}\u0001\u0000\u0000\u0000\u0014\u0080\u0001"+
		"\u0000\u0000\u0000\u0016\u0087\u0001\u0000\u0000\u0000\u0018\u0092\u0001"+
		"\u0000\u0000\u0000\u001a\u00a8\u0001\u0000\u0000\u0000\u001c\u00aa\u0001"+
		"\u0000\u0000\u0000\u001e\u00b5\u0001\u0000\u0000\u0000 \u00b7\u0001\u0000"+
		"\u0000\u0000\"\u00c6\u0001\u0000\u0000\u0000$\'\u0003\u0002\u0001\u0000"+
		"%\'\u0003\u0004\u0002\u0000&$\u0001\u0000\u0000\u0000&%\u0001\u0000\u0000"+
		"\u0000\'(\u0001\u0000\u0000\u0000(&\u0001\u0000\u0000\u0000()\u0001\u0000"+
		"\u0000\u0000)*\u0001\u0000\u0000\u0000*+\u0005\u0000\u0000\u0001+\u0001"+
		"\u0001\u0000\u0000\u0000,/\u0003\u0006\u0003\u0000-/\u0003\n\u0005\u0000"+
		".,\u0001\u0000\u0000\u0000.-\u0001\u0000\u0000\u0000/\u0003\u0001\u0000"+
		"\u0000\u000001\u0005\u0019\u0000\u000012\u0005\u001c\u0000\u000023\u0005"+
		"\b\u0000\u000034\u0005\t\u0000\u000045\u0003\u0016\u000b\u00005\u0005"+
		"\u0001\u0000\u0000\u000067\u0005\u001b\u0000\u000078\u0003\f\u0006\u0000"+
		"8=\u0003\b\u0004\u00009:\u0005\u0001\u0000\u0000:<\u0003\b\u0004\u0000"+
		";9\u0001\u0000\u0000\u0000<?\u0001\u0000\u0000\u0000=;\u0001\u0000\u0000"+
		"\u0000=>\u0001\u0000\u0000\u0000>@\u0001\u0000\u0000\u0000?=\u0001\u0000"+
		"\u0000\u0000@A\u0005\u0002\u0000\u0000A\u0007\u0001\u0000\u0000\u0000"+
		"BE\u0005\u001c\u0000\u0000CE\u0003\u0010\b\u0000DB\u0001\u0000\u0000\u0000"+
		"DC\u0001\u0000\u0000\u0000EF\u0001\u0000\u0000\u0000FT\u0005\u0003\u0000"+
		"\u0000GU\u0003\"\u0011\u0000HQ\u0005\u0006\u0000\u0000IN\u0003\"\u0011"+
		"\u0000JK\u0005\u0001\u0000\u0000KM\u0003\"\u0011\u0000LJ\u0001\u0000\u0000"+
		"\u0000MP\u0001\u0000\u0000\u0000NL\u0001\u0000\u0000\u0000NO\u0001\u0000"+
		"\u0000\u0000OR\u0001\u0000\u0000\u0000PN\u0001\u0000\u0000\u0000QI\u0001"+
		"\u0000\u0000\u0000QR\u0001\u0000\u0000\u0000RS\u0001\u0000\u0000\u0000"+
		"SU\u0005\u0007\u0000\u0000TG\u0001\u0000\u0000\u0000TH\u0001\u0000\u0000"+
		"\u0000U\t\u0001\u0000\u0000\u0000VW\u0003\f\u0006\u0000W\\\u0003\u000e"+
		"\u0007\u0000XY\u0005\u0001\u0000\u0000Y[\u0003\u000e\u0007\u0000ZX\u0001"+
		"\u0000\u0000\u0000[^\u0001\u0000\u0000\u0000\\Z\u0001\u0000\u0000\u0000"+
		"\\]\u0001\u0000\u0000\u0000]_\u0001\u0000\u0000\u0000^\\\u0001\u0000\u0000"+
		"\u0000_`\u0005\u0002\u0000\u0000`\u000b\u0001\u0000\u0000\u0000ab\u0005"+
		"\u0018\u0000\u0000b\r\u0001\u0000\u0000\u0000cf\u0005\u001c\u0000\u0000"+
		"df\u0003\u0010\b\u0000ec\u0001\u0000\u0000\u0000ed\u0001\u0000\u0000\u0000"+
		"fw\u0001\u0000\u0000\u0000gu\u0005\u0003\u0000\u0000hv\u0003\"\u0011\u0000"+
		"ir\u0005\u0006\u0000\u0000jo\u0003\"\u0011\u0000kl\u0005\u0001\u0000\u0000"+
		"ln\u0003\"\u0011\u0000mk\u0001\u0000\u0000\u0000nq\u0001\u0000\u0000\u0000"+
		"om\u0001\u0000\u0000\u0000op\u0001\u0000\u0000\u0000ps\u0001\u0000\u0000"+
		"\u0000qo\u0001\u0000\u0000\u0000rj\u0001\u0000\u0000\u0000rs\u0001\u0000"+
		"\u0000\u0000st\u0001\u0000\u0000\u0000tv\u0005\u0007\u0000\u0000uh\u0001"+
		"\u0000\u0000\u0000ui\u0001\u0000\u0000\u0000vx\u0001\u0000\u0000\u0000"+
		"wg\u0001\u0000\u0000\u0000wx\u0001\u0000\u0000\u0000x\u000f\u0001\u0000"+
		"\u0000\u0000y|\u0003\u0012\t\u0000z|\u0003\u0014\n\u0000{y\u0001\u0000"+
		"\u0000\u0000{z\u0001\u0000\u0000\u0000|\u0011\u0001\u0000\u0000\u0000"+
		"}~\u0005\u001a\u0000\u0000~\u007f\u0003\u0014\n\u0000\u007f\u0013\u0001"+
		"\u0000\u0000\u0000\u0080\u0081\u0005\u001c\u0000\u0000\u0081\u0083\u0005"+
		"\u0004\u0000\u0000\u0082\u0084\u0003\"\u0011\u0000\u0083\u0082\u0001\u0000"+
		"\u0000\u0000\u0083\u0084\u0001\u0000\u0000\u0000\u0084\u0085\u0001\u0000"+
		"\u0000\u0000\u0085\u0086\u0005\u0005\u0000\u0000\u0086\u0015\u0001\u0000"+
		"\u0000\u0000\u0087\u008b\u0005\u0006\u0000\u0000\u0088\u008a\u0003\u0018"+
		"\f\u0000\u0089\u0088\u0001\u0000\u0000\u0000\u008a\u008d\u0001\u0000\u0000"+
		"\u0000\u008b\u0089\u0001\u0000\u0000\u0000\u008b\u008c\u0001\u0000\u0000"+
		"\u0000\u008c\u008e\u0001\u0000\u0000\u0000\u008d\u008b\u0001\u0000\u0000"+
		"\u0000\u008e\u008f\u0005\u0007\u0000\u0000\u008f\u0017\u0001\u0000\u0000"+
		"\u0000\u0090\u0093\u0003\u0002\u0001\u0000\u0091\u0093\u0003\u001a\r\u0000"+
		"\u0092\u0090\u0001\u0000\u0000\u0000\u0092\u0091\u0001\u0000\u0000\u0000"+
		"\u0093\u0019\u0001\u0000\u0000\u0000\u0094\u00a9\u0003\u0016\u000b\u0000"+
		"\u0095\u00a9\u0005\u0002\u0000\u0000\u0096\u0097\u0003\"\u0011\u0000\u0097"+
		"\u0098\u0005\u0002\u0000\u0000\u0098\u00a9\u0001\u0000\u0000\u0000\u0099"+
		"\u009a\u0005\n\u0000\u0000\u009a\u009b\u0005\b\u0000\u0000\u009b\u009c"+
		"\u0003\u001c\u000e\u0000\u009c\u009d\u0005\t\u0000\u0000\u009d\u00a0\u0003"+
		"\u001a\r\u0000\u009e\u009f\u0005\u000b\u0000\u0000\u009f\u00a1\u0003\u001a"+
		"\r\u0000\u00a0\u009e\u0001\u0000\u0000\u0000\u00a0\u00a1\u0001\u0000\u0000"+
		"\u0000\u00a1\u00a9\u0001\u0000\u0000\u0000\u00a2\u00a3\u0005\f\u0000\u0000"+
		"\u00a3\u00a4\u0005\b\u0000\u0000\u00a4\u00a5\u0003\u001c\u000e\u0000\u00a5"+
		"\u00a6\u0005\t\u0000\u0000\u00a6\u00a7\u0003\u001a\r\u0000\u00a7\u00a9"+
		"\u0001\u0000\u0000\u0000\u00a8\u0094\u0001\u0000\u0000\u0000\u00a8\u0095"+
		"\u0001\u0000\u0000\u0000\u00a8\u0096\u0001\u0000\u0000\u0000\u00a8\u0099"+
		"\u0001\u0000\u0000\u0000\u00a8\u00a2\u0001\u0000\u0000\u0000\u00a9\u001b"+
		"\u0001\u0000\u0000\u0000\u00aa\u00ab\u0003\"\u0011\u0000\u00ab\u001d\u0001"+
		"\u0000\u0000\u0000\u00ac\u00b6\u0005\u001c\u0000\u0000\u00ad\u00ae\u0005"+
		"\u001c\u0000\u0000\u00ae\u00af\u0005\u0004\u0000\u0000\u00af\u00b0\u0003"+
		"\"\u0011\u0000\u00b0\u00b1\u0005\u0005\u0000\u0000\u00b1\u00b6\u0001\u0000"+
		"\u0000\u0000\u00b2\u00b3\u0005\u001c\u0000\u0000\u00b3\u00b4\u0005\b\u0000"+
		"\u0000\u00b4\u00b6\u0005\t\u0000\u0000\u00b5\u00ac\u0001\u0000\u0000\u0000"+
		"\u00b5\u00ad\u0001\u0000\u0000\u0000\u00b5\u00b2\u0001\u0000\u0000\u0000"+
		"\u00b6\u001f\u0001\u0000\u0000\u0000\u00b7\u00b8\u0005\u001d\u0000\u0000"+
		"\u00b8!\u0001\u0000\u0000\u0000\u00b9\u00ba\u0006\u0011\uffff\uffff\u0000"+
		"\u00ba\u00bb\u0003\u001e\u000f\u0000\u00bb\u00bc\u0005\u0003\u0000\u0000"+
		"\u00bc\u00bd\u0003\"\u0011\u0007\u00bd\u00c7\u0001\u0000\u0000\u0000\u00be"+
		"\u00c7\u0003\u001e\u000f\u0000\u00bf\u00c7\u0003 \u0010\u0000\u00c0\u00c1"+
		"\u0005\b\u0000\u0000\u00c1\u00c2\u0003\"\u0011\u0000\u00c2\u00c3\u0005"+
		"\t\u0000\u0000\u00c3\u00c7\u0001\u0000\u0000\u0000\u00c4\u00c5\u0007\u0000"+
		"\u0000\u0000\u00c5\u00c7\u0003\"\u0011\u0002\u00c6\u00b9\u0001\u0000\u0000"+
		"\u0000\u00c6\u00be\u0001\u0000\u0000\u0000\u00c6\u00bf\u0001\u0000\u0000"+
		"\u0000\u00c6\u00c0\u0001\u0000\u0000\u0000\u00c6\u00c4\u0001\u0000\u0000"+
		"\u0000\u00c7\u00d0\u0001\u0000\u0000\u0000\u00c8\u00c9\n\u0003\u0000\u0000"+
		"\u00c9\u00ca\u0007\u0001\u0000\u0000\u00ca\u00cf\u0003\"\u0011\u0004\u00cb"+
		"\u00cc\n\u0001\u0000\u0000\u00cc\u00cd\u0007\u0002\u0000\u0000\u00cd\u00cf"+
		"\u0003\"\u0011\u0002\u00ce\u00c8\u0001\u0000\u0000\u0000\u00ce\u00cb\u0001"+
		"\u0000\u0000\u0000\u00cf\u00d2\u0001\u0000\u0000\u0000\u00d0\u00ce\u0001"+
		"\u0000\u0000\u0000\u00d0\u00d1\u0001\u0000\u0000\u0000\u00d1#\u0001\u0000"+
		"\u0000\u0000\u00d2\u00d0\u0001\u0000\u0000\u0000\u0018&(.=DNQT\\eoruw"+
		"{\u0083\u008b\u0092\u00a0\u00a8\u00b5\u00c6\u00ce\u00d0";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}