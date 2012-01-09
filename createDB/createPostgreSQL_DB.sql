--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

--
-- Name: plpgsql; Type: PROCEDURAL LANGUAGE; Schema: -; Owner: postgres
--

CREATE PROCEDURAL LANGUAGE plpgsql;


ALTER PROCEDURAL LANGUAGE plpgsql OWNER TO postgres;

SET search_path = public, pg_catalog;

--
-- Name: delete_mail(); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION delete_mail() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	DELETE FROM mail_status WHERE mail_id=OLD.mail_id;
	RETURN old;
END;
$$;


ALTER FUNCTION public.delete_mail() OWNER TO postgres;

--
-- Name: delete_msg(integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION delete_msg(integer) RETURNS integer
    LANGUAGE plpgsql
    AS $_$
DECLARE
	id ALIAS FOR $1;
	attch RECORD;
	cnt integer;
	o oid;
BEGIN
  DELETE FROM notes WHERE mail_id=id;
  DELETE FROM mail_addresses WHERE mail_id=id;
  DELETE FROM header WHERE mail_id=id;
  DELETE FROM body WHERE mail_id=id;
  DELETE FROM mail_tags WHERE mail_id=id;

  FOR attch IN SELECT a.attachment_id,c.content,c.fingerprint FROM attachments a, attachment_contents c WHERE a.mail_id=id AND c.attachment_id=a.attachment_id LOOP
    cnt=0;
    IF attch.fingerprint IS NOT NULL THEN
      -- check if that content is shared with another message's attachment
      SELECT count(*) INTO cnt FROM attachment_contents WHERE fingerprint=attch.fingerprint AND attachment_id!=attch.attachment_id;
    END IF;
    IF (cnt=0) THEN
      PERFORM lo_unlink(attch.content);
    END IF;
    DELETE FROM attachment_contents WHERE attachment_id=attch.attachment_id;
  END LOOP;

  DELETE FROM attachments WHERE mail_id=id;
  UPDATE mail SET in_reply_to=NULL WHERE in_reply_to=id;

  SELECT mail_text INTO o FROM raw_mail WHERE mail_id=id;
  IF FOUND THEN
     PERFORM lo_unlink(o);
     DELETE FROM raw_mail WHERE mail_id=id;
  END IF;

  DELETE FROM mail WHERE mail_id=id;
  IF (FOUND) THEN
	  RETURN 1;
  ELSE
	  RETURN 0;
  END IF;
END;
$_$;


ALTER FUNCTION public.delete_msg(integer) OWNER TO postgres;

--
-- Name: delete_msg_set(integer[]); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION delete_msg_set(in_array_mail_id integer[]) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
 cnt int;
BEGIN
 cnt:=0;
 FOR idx IN array_lower(in_array_mail_id,1)..array_upper(in_array_mail_id,1) LOOP
   cnt:=cnt + delete_msg(in_array_mail_id[idx]);
 END LOOP;
 RETURN cnt;
END;
$$;


ALTER FUNCTION public.delete_msg_set(in_array_mail_id integer[]) OWNER TO postgres;

--
-- Name: insert_mail(); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION insert_mail() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF NEW.status&(256+32+16)=0 THEN
	  -- The message is not yet sent, archived, or trashed
	  INSERT INTO mail_status(mail_id,status) VALUES(new.mail_id,new.status);
	END IF;
	RETURN new;
END;
$$;


ALTER FUNCTION public.insert_mail() OWNER TO postgres;

--
-- Name: trash_msg(integer, integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION trash_msg(in_mail_id integer, in_op integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
new_status int;
BEGIN
  UPDATE mail SET status=status|16,mod_user_id=in_op WHERE mail_id=in_mail_id;
  SELECT INTO new_status status FROM mail WHERE mail_id=in_mail_id;
  RETURN new_status;
END;
$$;


ALTER FUNCTION public.trash_msg(in_mail_id integer, in_op integer) OWNER TO postgres;

--
-- Name: trash_msg_set(integer[], integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION trash_msg_set(in_array_mail_id integer[], in_op integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
cnt int;
BEGIN
  UPDATE mail SET status=status|16, mod_user_id=in_op WHERE mail_id=any(in_array_mail_id);
  GET DIAGNOSTICS cnt=ROW_COUNT;
  return cnt;
END;
$$;


ALTER FUNCTION public.trash_msg_set(in_array_mail_id integer[], in_op integer) OWNER TO postgres;

--
-- Name: untrash_msg(integer, integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION untrash_msg(in_mail_id integer, in_op integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
new_status int;
BEGIN
  UPDATE mail SET status=status&(~16),mod_user_id=in_op WHERE mail_id=in_mail_id;
  SELECT INTO new_status status FROM mail WHERE mail_id=in_mail_id;
  RETURN new_status;
END;
$$;


ALTER FUNCTION public.untrash_msg(in_mail_id integer, in_op integer) OWNER TO postgres;

--
-- Name: update_mail(); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION update_mail() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE
 rc int4;
BEGIN
   IF new.status!=old.status THEN
	IF NEW.status&(256+32+16)=0 THEN
	  -- The message is not yet sent, archived, or trashed
	  UPDATE mail_status
	    SET status = new.status
	   WHERE mail_id = new.mail_id;
	  GET DIAGNOSTICS rc = ROW_COUNT;
	  if rc=0 THEN
	    INSERT INTO mail_status(mail_id,status) VALUES(new.mail_id,new.status);
	  END IF;
	ELSE
	  -- The mail has been "processed"
	  DELETE FROM mail_status
	   WHERE mail_id = new.mail_id;
	END IF;
   END IF;
   RETURN new;
END;
$$;


ALTER FUNCTION public.update_mail() OWNER TO postgres;

--
-- Name: update_note_flag(); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION update_note_flag() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
  IF (TG_OP = 'DELETE') THEN
    UPDATE mail SET flags=flags&(~2) WHERE mail_id=OLD.mail_id;
    RETURN OLD;
  ELSIF (TG_OP = 'INSERT') THEN
    UPDATE mail SET flags=flags|2 WHERE mail_id=NEW.mail_id;
    RETURN NEW;
  END IF;
END;
$$;


ALTER FUNCTION public.update_note_flag() OWNER TO postgres;

--
-- Name: seq_addr_id; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE seq_addr_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.seq_addr_id OWNER TO postgres;

--
-- Name: seq_addr_id; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('seq_addr_id', 3, true);


SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: addresses; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE addresses (
    addr_id integer DEFAULT nextval('seq_addr_id'::regclass) NOT NULL,
    email_addr character varying(300),
    name character varying(300),
    nickname character varying(300),
    last_sent_to timestamp with time zone,
    last_recv_from timestamp with time zone,
    notes text,
    invalid integer DEFAULT 0,
    recv_pri integer DEFAULT 0,
    nb_sent_to integer,
    nb_recv_from integer
);


ALTER TABLE public.addresses OWNER TO postgres;

--
-- Name: attachment_contents; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE attachment_contents (
    attachment_id integer,
    content oid,
    fingerprint text
);


ALTER TABLE public.attachment_contents OWNER TO postgres;

--
-- Name: seq_attachment_id; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE seq_attachment_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.seq_attachment_id OWNER TO postgres;

--
-- Name: seq_attachment_id; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('seq_attachment_id', 1, true);


--
-- Name: attachments; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE attachments (
    attachment_id integer DEFAULT nextval('seq_attachment_id'::regclass) NOT NULL,
    mail_id integer,
    content_type character varying(300),
    content_size integer,
    filename character varying(300),
    charset character varying(30),
    mime_content_id text
);


ALTER TABLE public.attachments OWNER TO postgres;

--
-- Name: body; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE body (
    mail_id integer,
    bodytext text,
    bodyhtml text
);


ALTER TABLE public.body OWNER TO postgres;

--
-- Name: config; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE config (
    conf_key character varying(100) NOT NULL,
    value text,
    conf_name character varying(100),
    date_update timestamp with time zone
);


ALTER TABLE public.config OWNER TO postgres;

--
-- Name: files; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE files (
    mail_id integer,
    filename character varying(300)
);


ALTER TABLE public.files OWNER TO postgres;

--
-- Name: filter_action; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE filter_action (
    expr_id integer,
    action_order smallint,
    action_arg text,
    action_type character varying(100)
);


ALTER TABLE public.filter_action OWNER TO postgres;

--
-- Name: filter_expr; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE filter_expr (
    expr_id integer NOT NULL,
    name character varying(100),
    user_lastmod integer,
    last_update timestamp with time zone DEFAULT now(),
    expression text,
    direction character(1) DEFAULT 'I'::bpchar
);


ALTER TABLE public.filter_expr OWNER TO postgres;

--
-- Name: filter_log; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE filter_log (
    expr_id integer,
    mail_id integer,
    hit_date timestamp with time zone DEFAULT now()
);


ALTER TABLE public.filter_log OWNER TO postgres;

--
-- Name: forward_addresses; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE forward_addresses (
    to_email_addr character varying(300),
    forward_to text
);


ALTER TABLE public.forward_addresses OWNER TO postgres;

--
-- Name: global_notepad; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE global_notepad (
    contents text,
    last_modified timestamp with time zone
);


ALTER TABLE public.global_notepad OWNER TO postgres;

--
-- Name: header; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE header (
    mail_id integer,
    lines text
);


ALTER TABLE public.header OWNER TO postgres;

--
-- Name: identities; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE identities (
    email_addr character varying(200) NOT NULL,
    username character varying(200),
    xface text,
    signature text
);


ALTER TABLE public.identities OWNER TO postgres;

--
-- Name: inverted_word_index; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE inverted_word_index (
    word_id integer,
    part_no integer,
    mailvec bytea,
    nz_offset integer DEFAULT 0
);


ALTER TABLE public.inverted_word_index OWNER TO postgres;

--
-- Name: jobs_queue; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE jobs_queue (
    job_id integer NOT NULL,
    mail_id integer,
    job_type character varying(4),
    job_args text
);


ALTER TABLE public.jobs_queue OWNER TO postgres;

--
-- Name: jobs_queue_job_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE jobs_queue_job_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.jobs_queue_job_id_seq OWNER TO postgres;

--
-- Name: jobs_queue_job_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE jobs_queue_job_id_seq OWNED BY jobs_queue.job_id;


--
-- Name: jobs_queue_job_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('jobs_queue_job_id_seq', 1, false);


--
-- Name: mail; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE mail (
    mail_id integer,
    sender text,
    toname text,
    sender_fullname text,
    subject text,
    msg_date timestamp with time zone DEFAULT now(),
    sender_date timestamp with time zone,
    mbox_id integer,
    status integer,
    mod_user_id integer,
    thread_id integer,
    message_id text,
    in_reply_to integer,
    date_processed timestamp with time zone,
    priority integer DEFAULT 0,
    flags integer DEFAULT 0
);


ALTER TABLE public.mail OWNER TO postgres;

--
-- Name: mail_addresses; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE mail_addresses (
    mail_id integer,
    addr_id integer,
    addr_type smallint,
    addr_pos smallint
);


ALTER TABLE public.mail_addresses OWNER TO postgres;

--
-- Name: mail_status; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE mail_status (
    mail_id integer,
    status integer
);


ALTER TABLE public.mail_status OWNER TO postgres;

--
-- Name: mail_tags; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE mail_tags (
    mail_id integer,
    tag integer,
    agent integer,
    date_insert timestamp with time zone DEFAULT now()
);


ALTER TABLE public.mail_tags OWNER TO postgres;

--
-- Name: mailboxes; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE mailboxes (
    mbox_id integer,
    name text
);


ALTER TABLE public.mailboxes OWNER TO postgres;

--
-- Name: mime_types; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE mime_types (
    suffix character varying(20) NOT NULL,
    mime_type character varying(100) NOT NULL
);


ALTER TABLE public.mime_types OWNER TO postgres;

--
-- Name: non_indexable_words; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE non_indexable_words (
    wordtext character varying(50)
);


ALTER TABLE public.non_indexable_words OWNER TO postgres;

--
-- Name: notes; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE notes (
    mail_id integer,
    note text,
    last_changed timestamp with time zone DEFAULT now()
);


ALTER TABLE public.notes OWNER TO postgres;

--
-- Name: programs; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE programs (
    program_name character varying(256),
    content_type character varying(256),
    conf_name character varying(100)
);


ALTER TABLE public.programs OWNER TO postgres;

--
-- Name: raw_mail; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE raw_mail (
    mail_id integer,
    mail_text oid
);


ALTER TABLE public.raw_mail OWNER TO postgres;

--
-- Name: runtime_info; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE runtime_info (
    rt_key character varying(100) NOT NULL,
    rt_value text
);


ALTER TABLE public.runtime_info OWNER TO postgres;

--
-- Name: seq_mail_id; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE seq_mail_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.seq_mail_id OWNER TO postgres;

--
-- Name: seq_mail_id; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('seq_mail_id', 7, true);


--
-- Name: seq_tag_id; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE seq_tag_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.seq_tag_id OWNER TO postgres;

--
-- Name: seq_tag_id; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('seq_tag_id', 1, true);


--
-- Name: seq_thread_id; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE seq_thread_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.seq_thread_id OWNER TO postgres;

--
-- Name: seq_thread_id; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('seq_thread_id', 1, false);


--
-- Name: seq_word_id; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE seq_word_id
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.seq_word_id OWNER TO postgres;

--
-- Name: seq_word_id; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('seq_word_id', 1, false);


--
-- Name: tags; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE tags (
    tag_id integer DEFAULT nextval('seq_tag_id'::regclass),
    name character varying(300),
    parent_id integer
);


ALTER TABLE public.tags OWNER TO postgres;

--
-- Name: tags_words; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE tags_words (
    tag_id integer,
    word_id integer,
    counter integer
);


ALTER TABLE public.tags_words OWNER TO postgres;

--
-- Name: user_queries; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE user_queries (
    title text NOT NULL,
    sql_stmt text
);


ALTER TABLE public.user_queries OWNER TO postgres;

--
-- Name: users; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE users (
    user_id integer NOT NULL,
    fullname character varying(300),
    login character varying(80),
    email text,
    custom_field1 text,
    custom_field2 text,
    custom_field3 text,
    CONSTRAINT users_user_id_check CHECK ((user_id > 0))
);


ALTER TABLE public.users OWNER TO postgres;

--
-- Name: words; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE words (
    word_id integer NOT NULL,
    wordtext character varying(50)
);


ALTER TABLE public.words OWNER TO postgres;

--
-- Name: job_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE jobs_queue ALTER COLUMN job_id SET DEFAULT nextval('jobs_queue_job_id_seq'::regclass);


--
-- Data for Name: addresses; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY addresses (addr_id, email_addr, name, nickname, last_sent_to, last_recv_from, notes, invalid, recv_pri, nb_sent_to, nb_recv_from) FROM stdin;
1	postgres@vniins.ru	postgres		2011-08-04 18:38:43.653164+04	\N		0	0	2	\N
3	boris@vniins.ru			2011-08-04 18:39:46.693713+04	\N		0	0	1	\N
2	test@vniins.ru			2011-08-11 15:14:27.448168+04	\N		0	0	3	\N
\.


--
-- Data for Name: attachment_contents; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY attachment_contents (attachment_id, content, fingerprint) FROM stdin;
1	17076	Ui649HOkYgaNG+eewAwFFfXMj7M
\.


--
-- Data for Name: attachments; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY attachments (attachment_id, mail_id, content_type, content_size, filename, charset, mime_content_id) FROM stdin;
1	6	application/octet-stream	2111	config.h	\N	\N
\.


--
-- Data for Name: body; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY body (mail_id, bodytext, bodyhtml) FROM stdin;
1	\nfgdhsjagfjdksf	\N
2	\nsdfghjkl;yrueiowq	\N
3	\nmail to postgres	\N
4	\nmail to test	\N
5	mail to boooooooris\n	\N
6	\n	\N
\.


--
-- Data for Name: config; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY config (conf_key, value, conf_name, date_update) FROM stdin;
body_clickable_urls	0	zemlyansky.vniins.ru-linux	2011-08-04 13:24:12.083866+04
date_format	DD/MM/YYYY HH:MI	zemlyansky.vniins.ru-linux	2011-08-04 13:24:12.083866+04
default_identity	postgres@vniins.ru	zemlyansky.vniins.ru-linux	2011-08-04 13:24:12.083866+04
fetch/auto_incorporate_new_results	0	zemlyansky.vniins.ru-linux	2011-08-04 13:24:12.083866+04
fetch/auto_refresh_messages_list	0	zemlyansky.vniins.ru-linux	2011-08-04 13:24:12.083866+04
display/body/preferred_format	html	zemlyansky.vniins.ru-linux	2011-08-04 13:24:12.083866+04
display/filters_trace	0	zemlyansky.vniins.ru-linux	\N
display/msglist/column0_size	280	zemlyansky.vniins.ru-linux	\N
display/msglist/column1_size	170	zemlyansky.vniins.ru-linux	\N
display/msglist/column2_size	32	zemlyansky.vniins.ru-linux	\N
display/msglist/column3_size	32	zemlyansky.vniins.ru-linux	\N
display/msglist/column4_size	24	zemlyansky.vniins.ru-linux	\N
display/msglist/column5_size	24	zemlyansky.vniins.ru-linux	\N
display/msglist/column6_size	115	zemlyansky.vniins.ru-linux	\N
display/msglist/columns_ordering	0123456	zemlyansky.vniins.ru-linux	\N
display/msglist/height	600	zemlyansky.vniins.ru-linux	\N
display/msglist/panel1_size	100	zemlyansky.vniins.ru-linux	\N
display/msglist/panel2_size	358	zemlyansky.vniins.ru-linux	\N
display/msglist/panel3_size	0	zemlyansky.vniins.ru-linux	\N
display/msglist/width	800	zemlyansky.vniins.ru-linux	\N
display/tags_in_headers	1	zemlyansky.vniins.ru-linux	\N
show_tags	1	zemlyansky.vniins.ru-linux	\N
display_threads	1	zemlyansky.vniins.ru-linux	\N
show_headers_level	1	zemlyansky.vniins.ru-linux	\N
\.


--
-- Data for Name: files; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY files (mail_id, filename) FROM stdin;
\.


--
-- Data for Name: filter_action; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY filter_action (expr_id, action_order, action_arg, action_type) FROM stdin;
\.


--
-- Data for Name: filter_expr; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY filter_expr (expr_id, name, user_lastmod, last_update, expression, direction) FROM stdin;
\.


--
-- Data for Name: filter_log; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY filter_log (expr_id, mail_id, hit_date) FROM stdin;
\.


--
-- Data for Name: forward_addresses; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY forward_addresses (to_email_addr, forward_to) FROM stdin;
\.


--
-- Data for Name: global_notepad; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY global_notepad (contents, last_modified) FROM stdin;
\.


--
-- Data for Name: header; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY header (mail_id, lines) FROM stdin;
1	From: "postgres" <postgres@vniins.ru>\nTo: postgres@vniins.ru\nSubject: qweryui\nDate: Thu, 04 Aug 2011 13:24:56 +0400\nMessage-Id: <d550bf37-15a1-4d26-aa66-cdf3879a7546@mm>\nX-Mailer: Manitou v1.0.2\n
2	From: "postgres" <postgres@vniins.ru>\nTo: test@vniins.ru\nSubject: 1234\nDate: Thu, 04 Aug 2011 14:12:57 +0400\nMessage-Id: <f363b843-6799-45f3-a330-83395b94e9aa@mm>\nX-Mailer: Manitou v1.0.2\n
3	From: "postgres" <postgres@vniins.ru>\nTo: "postgres" <postgres@vniins.ru>\nSubject: postgres\nDate: Thu, 04 Aug 2011 18:38:43 +0400\nMessage-Id: <398330e3-945b-4ae9-94f3-849d7b4e64e5@mm>\nX-Mailer: Manitou v1.0.2\n
4	From: "postgres" <postgres@vniins.ru>\nTo: test@vniins.ru\nSubject: test\nDate: Thu, 04 Aug 2011 18:38:59 +0400\nMessage-Id: <c8eb7129-faa6-4fae-83b8-63f39967f3a5@mm>\nX-Mailer: Manitou v1.0.2\n
5	From: "postgres" <postgres@vniins.ru>\nTo: boris@vniins.ru\nSubject: boris\nDate: Thu, 04 Aug 2011 18:39:46 +0400\nMessage-Id: <79914bd3-4fcd-4dcd-8563-1dcb70856ff5@mm>\nX-Mailer: Manitou v1.0.2\n
6	From: "postgres" <postgres@vniins.ru>\nTo: test@vniins.ru\nSubject: attach\n
\.


--
-- Data for Name: identities; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY identities (email_addr, username, xface, signature) FROM stdin;
postgres@vniins.ru	postgres		
\.


--
-- Data for Name: inverted_word_index; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY inverted_word_index (word_id, part_no, mailvec, nz_offset) FROM stdin;
\.


--
-- Data for Name: jobs_queue; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY jobs_queue (job_id, mail_id, job_type, job_args) FROM stdin;
\.


--
-- Data for Name: mail; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY mail (mail_id, sender, toname, sender_fullname, subject, msg_date, sender_date, mbox_id, status, mod_user_id, thread_id, message_id, in_reply_to, date_processed, priority, flags) FROM stdin;
1	postgres@vniins.ru	\N	postgres	qweryui	2011-08-04 13:24:56.789673+04	2011-08-04 13:24:56.789673+04	\N	145	1	\N	d550bf37-15a1-4d26-aa66-cdf3879a7546@mm	\N	\N	0	0
2	postgres@vniins.ru	\N	postgres	1234	2011-08-04 14:12:57.229489+04	2011-08-04 14:12:57.229489+04	\N	177	1	\N	f363b843-6799-45f3-a330-83395b94e9aa@mm	\N	\N	0	0
3	postgres@vniins.ru	\N	postgres	postgres	2011-08-04 18:38:43.653164+04	2011-08-04 18:38:43.653164+04	\N	129	1	\N	398330e3-945b-4ae9-94f3-849d7b4e64e5@mm	\N	\N	0	0
4	postgres@vniins.ru	\N	postgres	test	2011-08-04 18:38:59.834486+04	2011-08-04 18:38:59.834486+04	\N	129	1	\N	c8eb7129-faa6-4fae-83b8-63f39967f3a5@mm	\N	\N	0	0
5	postgres@vniins.ru	\N	postgres	boris	2011-08-04 18:39:46.693713+04	2011-08-04 18:39:46.693713+04	\N	129	1	\N	79914bd3-4fcd-4dcd-8563-1dcb70856ff5@mm	\N	\N	0	0
6	postgres@vniins.ru	\N	postgres	attach	2011-08-11 15:14:27.448168+04	2011-08-11 15:14:27.448168+04	\N	129	1	\N	398330e3-945b-4ae9-94f3-849d7b4e64e5@mm	\N	\N	0	1
\.


--
-- Data for Name: mail_addresses; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY mail_addresses (mail_id, addr_id, addr_type, addr_pos) FROM stdin;
1	1	1	0
1	1	2	0
2	1	1	0
2	2	2	0
3	1	1	0
3	1	2	0
4	1	1	0
4	2	2	0
5	1	1	0
5	3	2	0
6	1	1	0
6	2	2	0
\.


--
-- Data for Name: mail_status; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY mail_status (mail_id, status) FROM stdin;
3	129
4	129
5	129
6	129
\.


--
-- Data for Name: mail_tags; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY mail_tags (mail_id, tag, agent, date_insert) FROM stdin;
4	1	1	2011-09-22 18:50:51.578987+04
5	1	1	2011-10-27 16:01:21.895377+04
\.


--
-- Data for Name: mailboxes; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY mailboxes (mbox_id, name) FROM stdin;
\.


--
-- Data for Name: mime_types; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY mime_types (suffix, mime_type) FROM stdin;
tar	application/x-tar
bmp	image/bmp
png	image/png
pdf	application/pdf
htm	text/html
txt	text/plain
zip	application/zip
html	text/html
jpg	image/jpeg
xml	text/xml
rtf	application/rtf
doc	application/msword
gif	image/gif
xls	application/vnd.ms-excel
\.


--
-- Data for Name: non_indexable_words; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY non_indexable_words (wordtext) FROM stdin;
\.


--
-- Data for Name: notes; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY notes (mail_id, note, last_changed) FROM stdin;
\.


--
-- Data for Name: programs; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY programs (program_name, content_type, conf_name) FROM stdin;
\.


--
-- Data for Name: raw_mail; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY raw_mail (mail_id, mail_text) FROM stdin;
\.


--
-- Data for Name: runtime_info; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY runtime_info (rt_key, rt_value) FROM stdin;
schema_version	1.0.2
\.


--
-- Data for Name: tags; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY tags (tag_id, name, parent_id) FROM stdin;
1	myTag	\N
\.


--
-- Data for Name: tags_words; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY tags_words (tag_id, word_id, counter) FROM stdin;
\.


--
-- Data for Name: user_queries; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY user_queries (title, sql_stmt) FROM stdin;
mail_subject_test	SELECT m.mail_id FROM mail m,mail_addresses ma1 WHERE ma1.addr_id='1' AND m.mail_id=ma1.mail_id AND status&16=0 AND m.subject='test'
all	SELECT m.mail_id FROM mail m 
\.


--
-- Data for Name: users; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY users (user_id, fullname, login, email, custom_field1, custom_field2, custom_field3) FROM stdin;
1	\N	postgres	\N	\N	\N	\N
\.


--
-- Data for Name: words; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY words (word_id, wordtext) FROM stdin;
\.


--
-- Data for Name: BLOBS; Type: BLOBS; Schema: -; Owner: 
--

SET search_path = pg_catalog;

BEGIN;

SELECT lo_open(lo_create(17076), 131072);
SELECT pg_catalog.lowrite(0, '/* config.h.  Generated from config.h.in by configure.  */\\012/* config.h.in.  Generated from configure.in by autoheader.  */\\012\\012/* Define to 1 if you have the declaration of `getopt_long'', and to 0 if you\\012   don''t. */\\012#define HAVE_DECL_GETOPT_LONG 1\\012\\012/* Define to 1 if you have the declaration of `strerror_r'', and to 0 if you\\012   don''t. */\\012#define HAVE_DECL_STRERROR_R 1\\012\\012/* Define to 1 if you have the <inttypes.h> header file. */\\012#define HAVE_INTTYPES_H 1\\012\\012/* Define to 1 if you have the <libpq-fe.h> header file. */\\012#define HAVE_LIBPQ_FE_H 1\\012\\012/* Define to 1 if you have the <memory.h> header file. */\\012#define HAVE_MEMORY_H 1\\012\\012/* Use postgres backend */\\012#define HAVE_PGSQL 1\\012\\012/* Define to 1 if you have the <stdint.h> header file. */\\012#define HAVE_STDINT_H 1\\012\\012/* Define to 1 if you have the <stdlib.h> header file. */\\012#define HAVE_STDLIB_H 1\\012\\012/* Define to 1 if you have the `strerror_r'' function. */\\012#define HAVE_STRERROR_R 1\\012\\012/* Define to 1 if you have the <strings.h> header file. */\\012#define HAVE_STRINGS_H 1\\012\\012/* Define to 1 if you have the <string.h> header file. */\\012#define HAVE_STRING_H 1\\012\\012/* Define to 1 if you have the <sys/stat.h> header file. */\\012#define HAVE_SYS_STAT_H 1\\012\\012/* Define to 1 if you have the <sys/types.h> header file. */\\012#define HAVE_SYS_TYPES_H 1\\012\\012/* Define to 1 if you have the <unistd.h> header file. */\\012#define HAVE_UNISTD_H 1\\012\\012/* Name of package */\\012#define PACKAGE "manitou"\\012\\012/* Define to the address where bug reports for this package should be sent. */\\012#define PACKAGE_BUGREPORT ""\\012\\012/* Define to the full name of this package. */\\012#define PACKAGE_NAME ""\\012\\012/* Define to the full name and version of this package. */\\012#define PACKAGE_STRING ""\\012\\012/* Define to the one symbol short name of this package. */\\012#define PACKAGE_TARNAME ""\\012\\012/* Define to the home page for this package. */\\012#define PACKAGE_URL ""\\012\\012/* Define to the version of this package. */\\012#define PACKAGE_VERSION ""\\012\\012/* Define to 1 if you have the ANSI C header files. */\\012#define STDC_HEADERS 1\\012\\012/* Define to 1 if strerror_r returns char *. */\\012#define STRERROR_R_CHAR_P 1\\012\\012/* Version number of package */\\012#define VERSION "1.0.2"\\012');
SELECT lo_close(0);

COMMIT;

--
-- Data for Name: BLOB COMMENTS; Type: BLOB COMMENTS; Schema: -; Owner: 
--


SET search_path = public, pg_catalog;

--
-- Name: addresses_email_addr_key; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY addresses
    ADD CONSTRAINT addresses_email_addr_key UNIQUE (email_addr);


--
-- Name: addresses_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY addresses
    ADD CONSTRAINT addresses_pkey PRIMARY KEY (addr_id);


--
-- Name: attachments_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY attachments
    ADD CONSTRAINT attachments_pkey PRIMARY KEY (attachment_id);


--
-- Name: filter_expr_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY filter_expr
    ADD CONSTRAINT filter_expr_pkey PRIMARY KEY (expr_id);


--
-- Name: users_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY users
    ADD CONSTRAINT users_pkey PRIMARY KEY (user_id);


--
-- Name: words_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY words
    ADD CONSTRAINT words_pkey PRIMARY KEY (word_id);


--
-- Name: attach_ct_fp_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX attach_ct_fp_idx ON attachment_contents USING btree (fingerprint);


--
-- Name: attch_ct_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX attch_ct_idx ON attachment_contents USING btree (attachment_id);


--
-- Name: expr_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX expr_idx ON filter_expr USING btree (name);


--
-- Name: filter_action_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX filter_action_idx ON filter_action USING btree (expr_id, action_order);


--
-- Name: fwa_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX fwa_idx ON forward_addresses USING btree (to_email_addr);


--
-- Name: idx_attachments_mail_id; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX idx_attachments_mail_id ON attachments USING btree (mail_id);


--
-- Name: idx_raw_mail; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX idx_raw_mail ON raw_mail USING btree (mail_id);


--
-- Name: iwi_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX iwi_idx ON inverted_word_index USING btree (word_id, part_no);


--
-- Name: jobs_pk_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX jobs_pk_idx ON jobs_queue USING btree (job_id);


--
-- Name: mail_addresses_addrid_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX mail_addresses_addrid_idx ON mail_addresses USING btree (addr_id);


--
-- Name: mail_addresses_mailid_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX mail_addresses_mailid_idx ON mail_addresses USING btree (mail_id);


--
-- Name: mail_date_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX mail_date_idx ON mail USING btree (msg_date);


--
-- Name: mail_in_replyto_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX mail_in_replyto_idx ON mail USING btree (in_reply_to);


--
-- Name: mail_message_id_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX mail_message_id_idx ON mail USING btree (message_id);


--
-- Name: mail_tags_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX mail_tags_idx ON mail_tags USING btree (mail_id, tag);


--
-- Name: mail_thread_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX mail_thread_idx ON mail USING btree (thread_id);


--
-- Name: mailboxes_mbox_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX mailboxes_mbox_idx ON mailboxes USING btree (mbox_id);


--
-- Name: mailboxes_mbox_name; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX mailboxes_mbox_name ON mailboxes USING btree (name);


--
-- Name: notes_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX notes_idx ON notes USING btree (mail_id);


--
-- Name: pk_body_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX pk_body_idx ON body USING btree (mail_id);


--
-- Name: pk_header_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX pk_header_idx ON header USING btree (mail_id);


--
-- Name: pk_mail_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX pk_mail_idx ON mail USING btree (mail_id);


--
-- Name: pk_mail_status_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX pk_mail_status_idx ON mail_status USING btree (mail_id);


--
-- Name: runtime_info_pk; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX runtime_info_pk ON runtime_info USING btree (rt_key);


--
-- Name: tag_id_pk; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX tag_id_pk ON tags USING btree (tag_id);


--
-- Name: tags_words_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE INDEX tags_words_idx ON tags_words USING btree (word_id);


--
-- Name: user_queries_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX user_queries_idx ON user_queries USING btree (title);


--
-- Name: users_login_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX users_login_idx ON users USING btree (login);


--
-- Name: wordtext_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX wordtext_idx ON words USING btree (wordtext);


--
-- Name: delete_mail; Type: TRIGGER; Schema: public; Owner: postgres
--

CREATE TRIGGER delete_mail
    AFTER DELETE ON mail
    FOR EACH ROW
    EXECUTE PROCEDURE delete_mail();


--
-- Name: insert_mail; Type: TRIGGER; Schema: public; Owner: postgres
--

CREATE TRIGGER insert_mail
    AFTER INSERT ON mail
    FOR EACH ROW
    EXECUTE PROCEDURE insert_mail();


--
-- Name: update_mail; Type: TRIGGER; Schema: public; Owner: postgres
--

CREATE TRIGGER update_mail
    AFTER UPDATE ON mail
    FOR EACH ROW
    EXECUTE PROCEDURE update_mail();


--
-- Name: update_note; Type: TRIGGER; Schema: public; Owner: postgres
--

CREATE TRIGGER update_note
    AFTER INSERT OR DELETE ON notes
    FOR EACH ROW
    EXECUTE PROCEDURE update_note_flag();


--
-- Name: attachment_contents_attachment_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY attachment_contents
    ADD CONSTRAINT attachment_contents_attachment_id_fkey FOREIGN KEY (attachment_id) REFERENCES attachments(attachment_id);


--
-- Name: attachments_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY attachments
    ADD CONSTRAINT attachments_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: body_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY body
    ADD CONSTRAINT body_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: filter_action_expr_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY filter_action
    ADD CONSTRAINT filter_action_expr_id_fkey FOREIGN KEY (expr_id) REFERENCES filter_expr(expr_id);


--
-- Name: header_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY header
    ADD CONSTRAINT header_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: inverted_word_index_word_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY inverted_word_index
    ADD CONSTRAINT inverted_word_index_word_id_fkey FOREIGN KEY (word_id) REFERENCES words(word_id);


--
-- Name: mail_addresses_addr_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mail_addresses
    ADD CONSTRAINT mail_addresses_addr_id_fkey FOREIGN KEY (addr_id) REFERENCES addresses(addr_id);


--
-- Name: mail_addresses_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mail_addresses
    ADD CONSTRAINT mail_addresses_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: mail_mbox_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mail
    ADD CONSTRAINT mail_mbox_id_fkey FOREIGN KEY (mbox_id) REFERENCES mailboxes(mbox_id);


--
-- Name: mail_tags_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mail_tags
    ADD CONSTRAINT mail_tags_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: mail_tags_tag_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY mail_tags
    ADD CONSTRAINT mail_tags_tag_fkey FOREIGN KEY (tag) REFERENCES tags(tag_id);


--
-- Name: notes_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY notes
    ADD CONSTRAINT notes_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: parent_tag_fk; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY tags
    ADD CONSTRAINT parent_tag_fk FOREIGN KEY (parent_id) REFERENCES tags(tag_id);


--
-- Name: raw_mail_mail_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY raw_mail
    ADD CONSTRAINT raw_mail_mail_id_fkey FOREIGN KEY (mail_id) REFERENCES mail(mail_id);


--
-- Name: tags_words_tag_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY tags_words
    ADD CONSTRAINT tags_words_tag_id_fkey FOREIGN KEY (tag_id) REFERENCES tags(tag_id);


--
-- Name: tags_words_word_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY tags_words
    ADD CONSTRAINT tags_words_word_id_fkey FOREIGN KEY (word_id) REFERENCES words(word_id);


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--

