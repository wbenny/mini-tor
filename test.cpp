#ifdef MINI_TEST

#define _CRTDBG_MAP_ALLOC
#include <mini/memory.h>
#include <stdlib.h>
#include <crtdbg.h>

#include <mini/collections/list.h>
#include <mini/collections/pair_list.h>
#include <mini/collections/linked_list.h>
#include <mini/collections/hashset.h>
#include <mini/collections/hashmap.h>
#include <mini/string.h>
#include <mini/console.h>

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include <cassert>

int __cdecl
main(
  int argc,
  char* argv[]
  )
{
  /*
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer
    pellentesque metus et sem condimentum, ut malesuada dolor hendrerit.
    Nam ultrices tempus mi ac congue. Vivamus auctor sit amet lorem non
    finibus. Praesent congue bibendum dui eget maximus. Sed vel ex vel nisl
    viverra tristique at finibus est. Vivamus in magna in mauris interdum
    luctus. Curabitur vel elit imperdiet, egestas neque at, rutrum massa.
    Nulla tincidunt at turpis vel consequat. Nulla metus nisl, mollis eget
    iaculis quis, consectetur ac nibh.
  */

  MINI_UNREFERENCED(argc);
  MINI_UNREFERENCED(argv);

  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  {
    mini::collections::list<int> list_int;
    list_int.add(1);
    list_int.add(2);
    list_int.add(3);
    list_int.add(4);
    list_int.add(5);

    assert(list_int.get_size() == 5);
    assert(list_int.get_capacity() == 20);
    assert(list_int.index_of(4) == 3);

  }

  _CrtDumpMemoryLeaks();

  {
    mini::collections::hashmap<int, mini::string> hashmap_int_string;
    hashmap_int_string[0x1234] = "Lorem ipsum";
    hashmap_int_string[0x5678] = "dolor sit";
    hashmap_int_string[0x4231] = "amet, consectetur";
    hashmap_int_string[0x1423] = "adipiscing elit";
    hashmap_int_string[0x3142] = "pellentesque metus";

    assert(hashmap_int_string.get_size() == 5);
    assert(hashmap_int_string.get_bucket_count() == 8);

    assert(hashmap_int_string[0x1234] == "Lorem ipsum");
    assert(hashmap_int_string[0x5678] == "dolor sit");
    assert(hashmap_int_string[0x4231] == "amet, consectetur");
    assert(hashmap_int_string[0x1423] == "adipiscing elit");
    assert(hashmap_int_string[0x3142] == "pellentesque metus");

    assert(hashmap_int_string[0x1234] != "!Lorem ipsum");
    assert(hashmap_int_string[0x5678] != "!dolor sit");
    assert(hashmap_int_string[0x4231] != "!amet, consectetur");
    assert(hashmap_int_string[0x1423] != "!adipiscing elit");
    assert(hashmap_int_string[0x3142] != "!pellentesque metus");

    assert(hashmap_int_string.find(0x1423) != hashmap_int_string.end());
    assert(hashmap_int_string.find(0x4444) == hashmap_int_string.end());

    hashmap_int_string.remove(hashmap_int_string.find(0x4231));

    assert(hashmap_int_string.get_size() == 4);
    assert(hashmap_int_string.find(0x4231) == hashmap_int_string.end());

    {
      auto copy_of_hashmap_int_string = hashmap_int_string;
      assert(copy_of_hashmap_int_string.get_size() == 4);
      assert(copy_of_hashmap_int_string.get_bucket_count() == 8);

      assert(copy_of_hashmap_int_string[0x1234] == "Lorem ipsum");
      assert(copy_of_hashmap_int_string[0x5678] == "dolor sit");
      assert(copy_of_hashmap_int_string[0x1423] == "adipiscing elit");
      assert(copy_of_hashmap_int_string[0x3142] == "pellentesque metus");

      assert(copy_of_hashmap_int_string[0x1234] != "!Lorem ipsum");
      assert(copy_of_hashmap_int_string[0x5678] != "!dolor sit");
      assert(copy_of_hashmap_int_string[0x1423] != "!adipiscing elit");
      assert(copy_of_hashmap_int_string[0x3142] != "!pellentesque metus");

      assert(copy_of_hashmap_int_string.find(0x5678) != copy_of_hashmap_int_string.end());
      assert(copy_of_hashmap_int_string.find(0x4444) == copy_of_hashmap_int_string.end());

      copy_of_hashmap_int_string.remove(copy_of_hashmap_int_string.find(0x5678));

      assert(copy_of_hashmap_int_string.get_size() == 3);
      assert(copy_of_hashmap_int_string.find(0x5678) == copy_of_hashmap_int_string.end());
    }

    {
      auto move_of_hashmap_int_string = std::move(hashmap_int_string);
      assert(move_of_hashmap_int_string.get_size() == 4);
      assert(move_of_hashmap_int_string.get_bucket_count() == 8);

      assert(move_of_hashmap_int_string[0x1234] == "Lorem ipsum");
      assert(move_of_hashmap_int_string[0x5678] == "dolor sit");
      assert(move_of_hashmap_int_string[0x1423] == "adipiscing elit");
      assert(move_of_hashmap_int_string[0x3142] == "pellentesque metus");

      assert(move_of_hashmap_int_string[0x1234] != "!Lorem ipsum");
      assert(move_of_hashmap_int_string[0x5678] != "!dolor sit");
      assert(move_of_hashmap_int_string[0x1423] != "!adipiscing elit");
      assert(move_of_hashmap_int_string[0x3142] != "!pellentesque metus");

      assert(move_of_hashmap_int_string.find(0x5678) != move_of_hashmap_int_string.end());
      assert(move_of_hashmap_int_string.find(0x4444) == move_of_hashmap_int_string.end());

      move_of_hashmap_int_string.remove(move_of_hashmap_int_string.find(0x5678));

      assert(move_of_hashmap_int_string.get_size() == 3);
      assert(move_of_hashmap_int_string.find(0x5678) == move_of_hashmap_int_string.end());
    }
  }

  {
    mini::collections::hashmap<mini::string, mini::string> hashmap_string_string;
    hashmap_string_string.insert_many({
      { "Lorem ipsum", "dolor sit" },
      { "amet, consectetur", "adipiscing elit." },
      { "pellentesque metus", " et sem condimentum" },
      { "ut malesuada ", "dolor hendrerit." },
      { "Nam ultrices", "tempus mi" }
      });

    for (auto&& kv : hashmap_string_string)
    {
      mini::console::write("'%s':'%s'\n", kv.first.get_buffer(), kv.second.get_buffer());
    }
  }

  _CrtDumpMemoryLeaks();

  return 0;
}

#endif
