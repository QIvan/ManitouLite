CREATE TABLE Sequence (
	name text,
	curr_val integer
);

INSERT INTO Sequence (name, curr_val) VALUES ('sequence_id', 1);

CREATE TABLE test_table (
	test_id INTEGER,
	test_text text,
	test_char200 character varying(200),
	test_date timestamp with time zone,
	test_int_not_null integer NOT NULL
);

CREATE PROCEDURAL LANGUAGE plpgsql;

CREATE FUNCTION autoincrement() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
begin
	UPDATE test_table SET test_id=(select curr_val+1 from Sequence where name='sequence_id') where test_id is null;
 	UPDATE Sequence SET curr_val=curr_val+1 		       where name='sequence_id';
  	return new;
end;
$$;
CREATE TRIGGER tautoincrement AFTER INSERT ON test_table
    FOR EACH ROW EXECUTE PROCEDURE autoincrement();

CREATE TABLE table_for_thread (
    field integer
);

