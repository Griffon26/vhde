entity top_entity is
end top_entity;

architecture top_arch of top_entity is

  component used_entity
    port (
      myport1: in sometype;
      myport2: inout sometype
    );
  end component;

  signal mysignal1: sometype;
  signal mysignal2: sometype;

begin
  myinstance1: used_entity
  port map (
    myport1 => mysignal1,
    myport2 => open
  );
  myinstance2: used_entity
  port map (
    myport1 => open,
    myport2 => open
  );
end;

