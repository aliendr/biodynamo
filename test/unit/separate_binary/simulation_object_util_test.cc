// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------

#include "gtest/gtest.h"

#include "simulation_implementation.h"
#include "unit/separate_binary/simulation_object_util_test.h"
#include "unit/test_util.h"

namespace bdm {
// namespace simulation_object_util_test_internal {

// The following tests check if code insertion in new classes works as intended
// Therefore SimulationObject is extended in two stages: first by CellExt and
// then by NeuronExt

TEST(SimulationObjectUtilTest, ContainerFunctionality) {
  using Scalar = ContainerTestClass;
  auto vector = Scalar::NewEmptySoa();

  EXPECT_EQ(0u, vector.size());
  EXPECT_EQ(0u, vector.GetTotalSize());

  EXPECT_EQ(0u, vector.DelayedPushBack(Scalar(1, 1.0)));
  EXPECT_EQ(1u, vector.DelayedPushBack(Scalar(2, 2.0)));
  EXPECT_EQ(2u, vector.DelayedPushBack(Scalar(3, 3.0)));

  // changes have not been commited yet
  EXPECT_EQ(0u, vector.size());
  //   but data is already in vector
  EXPECT_EQ(3u, vector.GetVecDm1().size());
  EXPECT_EQ(3u, vector.GetVecDm2().size());
  EXPECT_EQ(3u, vector.GetTotalSize());

  vector.Commit();

  EXPECT_EQ(3u, vector.size());
  EXPECT_EQ(3u, vector.GetVecDm1().size());
  EXPECT_EQ(3u, vector.GetVecDm2().size());
  EXPECT_EQ(3u, vector.GetTotalSize());

  EXPECT_EQ(1, vector[0].GetDm1());
  EXPECT_EQ(1.0, vector[0].GetDm2());
  EXPECT_EQ(2, vector[1].GetDm1());
  EXPECT_EQ(2.0, vector[1].GetDm2());
  EXPECT_EQ(3, vector[2].GetDm1());
  EXPECT_EQ(3.0, vector[2].GetDm2());

  vector.DelayedRemove(0);

  // changes have not been commited yet
  EXPECT_EQ(3u, vector.size());
  EXPECT_EQ(3u, vector.GetVecDm1().size());
  EXPECT_EQ(3u, vector.GetVecDm2().size());
  EXPECT_EQ(3u, vector.GetTotalSize());

  vector.Commit();

  EXPECT_EQ(2u, vector.size());
  EXPECT_EQ(2u, vector.GetVecDm1().size());
  EXPECT_EQ(2u, vector.GetVecDm2().size());
  EXPECT_EQ(2u, vector.GetTotalSize());

  EXPECT_EQ(3, vector[0].GetDm1());
  EXPECT_EQ(3.0, vector[0].GetDm2());
  EXPECT_EQ(2, vector[1].GetDm1());
  EXPECT_EQ(2.0, vector[1].GetDm2());

  vector.DelayedRemove(0);
  vector.DelayedRemove(1);
  vector.Commit();
  EXPECT_EQ(0u, vector.size());
  EXPECT_EQ(0u, vector.GetTotalSize());

  // push_back
  vector.push_back(Scalar(9, 9.0));
  EXPECT_EQ(1u, vector.size());
  EXPECT_EQ(1u, vector.GetTotalSize());
  EXPECT_EQ(9, vector[0].GetDm1());

  vector.DelayedPushBack(Scalar(10, 10.0));
  EXPECT_EQ(1u, vector.size());
  EXPECT_EQ(2u, vector.GetTotalSize());
  // clang on Travis OSX image doesn't catch exception
  // Therefore the following check is commented until this is fixed
  // try {
  //   vector.push_back(Scalar(11, 11.0));
  //   FAIL() << "Should have thrown a logic_error";
  // } catch(std::logic_error& e) {}
}

template <typename T>
void RunDefaultConstructorTest(const T& neuron) {
  EXPECT_EQ(1u, neuron.size());

  EXPECT_EQ(6.28, neuron.GetDiameter());
  auto& position = neuron.GetPosition();
  EXPECT_EQ(1, position[0]);
  EXPECT_EQ(2, position[1]);
  EXPECT_EQ(3, position[2]);
  auto neurites = neuron.GetNeurites();
  EXPECT_EQ(0u, neurites.size());
}

TEST(SimulationObjectUtilTest, DefaultConstructor) {
  // are data members in all extensions correctly initialized?
  Neuron neuron;
  RunDefaultConstructorTest(Neuron());
  RunDefaultConstructorTest(SoaNeuron());
}

TEST(SimulationObjectUtilTest, NewEmptySoa) {
  // Create an empty SOA container
  // Creating it using e.g. `SoaNeuron soa;` will already have one element
  // inside - (the one with default parameters)
  auto neurons = Neuron::NewEmptySoa();
  EXPECT_EQ(0u, neurons.size());
  EXPECT_EQ(0u, neurons.neurites_.size());
  EXPECT_EQ(0u, neurons.diameter_.size());
  EXPECT_EQ(0u, neurons.position_.size());
}

TEST(SimulationObjectUtilTest, NonDefaultConstructor) {
  std::vector<Neurite> neurites;
  neurites.push_back(Neurite(2));
  neurites.push_back(Neurite(3));

  Neuron neuron(neurites, std::array<double, 3>{4, 5, 6});

  EXPECT_EQ(6.28, neuron.GetDiameter());
  auto& position = neuron.GetPosition();
  EXPECT_EQ(4, position[0]);
  EXPECT_EQ(5, position[1]);
  EXPECT_EQ(6, position[2]);
  EXPECT_EQ(2u, neurites.size());
}

TEST(SimulationObjectUtilTest, SoaRef) {
  SoaNeuron neurons;

  auto neurons_ref = neurons[0];

  EXPECT_EQ(1u, neurons_ref.size());

  // check if changes are visible for the referenced object
  neurons_ref.SetDiameter(12.34);
  EXPECT_EQ(12.34, neurons.GetDiameter());
  neurons_ref.push_back(Neuron());
  EXPECT_EQ(2u, neurons.size());
}

TEST(SimulationObjectUtilTest, Soa_push_back_AndSubscriptOperator) {
  std::vector<Neurite> neurites;
  neurites.push_back(Neurite(2));
  neurites.push_back(Neurite(3));

  Neuron neuron1(neurites, std::array<double, 3>{4, 5, 6});

  neurites.push_back(Neurite(4));
  Neuron neuron2(neurites, std::array<double, 3>{9, 8, 7});

  auto neurons = Neuron::NewEmptySoa();
  neurons.push_back(neuron1);
  neurons.push_back(neuron2);

  EXPECT_EQ(2u, neurons.size());

  EXPECT_EQ(6.28, neurons[0].GetDiameter());
  auto& position1 = neurons[0].GetPosition();
  EXPECT_EQ(4, position1[0]);
  EXPECT_EQ(5, position1[1]);
  EXPECT_EQ(6, position1[2]);
  EXPECT_EQ(2u, neurons[0].GetNeurites().size());

  // test if return type of subscript operator has SoaRef backend
  auto element1 = neurons[1];
  Neuron::Self<SoaRef>* cast_result =
      dynamic_cast<Neuron::Self<SoaRef>*>(&element1);
  EXPECT_TRUE(cast_result != nullptr);

  EXPECT_EQ(6.28, neurons[1].GetDiameter());
  auto& position2 = neurons[1].GetPosition();
  EXPECT_EQ(9, position2[0]);
  EXPECT_EQ(8, position2[1]);
  EXPECT_EQ(7, position2[2]);
  EXPECT_EQ(3u, neurons[1].GetNeurites().size());

  // Test soa.push_back(soa_ref)
  auto neurons2 = Neuron::NewEmptySoa();
  neurons2.push_back(neurons[1]);

  EXPECT_EQ(6.28, neurons2[0].GetDiameter());
  auto& pos = neurons2[0].GetPosition();
  EXPECT_EQ(9, pos[0]);
  EXPECT_EQ(8, pos[1]);
  EXPECT_EQ(7, pos[2]);
  EXPECT_EQ(3u, neurons2[0].GetNeurites().size());
}

TEST(SimulationObjectUtilTest, Soa_clear) {
  SoaNeuron neurons;
  EXPECT_EQ(1u, neurons.size());
  neurons.clear();
  EXPECT_EQ(0u, neurons.size());
  EXPECT_EQ(0u, neurons.neurites_.size());
  EXPECT_EQ(0u, neurons.diameter_.size());
  EXPECT_EQ(0u, neurons.position_.size());
}

TEST(SimulationObjectUtilTest, Soa_reserve) {
  SoaNeuron neurons;
  neurons.reserve(10);
  EXPECT_EQ(10u, neurons.neurites_.capacity());
  EXPECT_EQ(10u, neurons.diameter_.capacity());
  EXPECT_EQ(10u, neurons.position_.capacity());
}

TEST(SimulationObjectUtilTest, Soa_AssignmentOperator) {
  std::vector<Neurite> neurites;
  neurites.push_back(Neurite(2));
  neurites.push_back(Neurite(3));

  Neuron neuron1(neurites, std::array<double, 3>{4, 5, 6});

  neurites.push_back(Neurite(4));
  Neuron new_neuron1(neurites, std::array<double, 3>{9, 8, 7});
  new_neuron1.SetDiameter(123);

  auto neurons = Neuron::NewEmptySoa();
  neurons.push_back(neuron1);

  EXPECT_EQ(1u, neurons.size());

  neurons[0] = new_neuron1;
  EXPECT_EQ(123u, neurons[0].GetDiameter());
  auto& position = neurons[0].GetPosition();
  EXPECT_EQ(9, position[0]);
  EXPECT_EQ(8, position[1]);
  EXPECT_EQ(7, position[2]);
  EXPECT_EQ(3u, neurons[0].GetNeurites().size());
}

TEST(SimulationObjectUtilTest, Soa_DelayedRemove) {
  auto vector = TestObject::NewEmptySoa();
  for (uint64_t i = 0; i < 10; i++) {
    vector.push_back(TestObject(i));
  }

  EXPECT_EQ(10u, vector.size());

  vector.DelayedRemove(5);
  vector.DelayedRemove(8);
  vector.DelayedRemove(3);

  EXPECT_EQ(10u, vector.size());

  auto updated_indices = vector.Commit();

  EXPECT_EQ(7u, vector.size());
  ASSERT_EQ(2u, updated_indices.size());
  EXPECT_EQ(5u, updated_indices[9]);
  EXPECT_EQ(3u, updated_indices[7]);

  EXPECT_EQ(0, vector[0].GetId());
  EXPECT_EQ(1, vector[1].GetId());
  EXPECT_EQ(2, vector[2].GetId());
  EXPECT_EQ(7, vector[3].GetId());
  EXPECT_EQ(4, vector[4].GetId());
  EXPECT_EQ(9, vector[5].GetId());
  EXPECT_EQ(6, vector[6].GetId());
}

// Tests overloaded Divide function which adds new daughter cell to the
// container managed by the ResourceManager with default template parameters
TEST(SimulationObjectUtilTest, Soa_DivideWithResourceManager) {
  Simulation<> simulation(TEST_NAME);
  auto* rm = simulation.GetResourceManager();

  auto neurons = rm->Get<Neuron>();
  Neuron neuron;
  neurons->push_back(neuron);

  auto new_neuron = (*neurons)[0].Divide(1.0, 2.0, 3.0);

  EXPECT_EQ(987u, new_neuron->GetNeurites()[0].id_);
  EXPECT_EQ(5, new_neuron->GetPosition()[0]);
  EXPECT_EQ(4, new_neuron->GetPosition()[1]);
  EXPECT_EQ(3, new_neuron->GetPosition()[2]);

  // commit invalidates new_neuron
  neurons->Commit();

  ASSERT_EQ(2u, neurons->size());
  // new_neuron got invalidated by `Commit()`, but is now accessible in neurons
  EXPECT_EQ(987u, (*neurons)[1].GetNeurites()[0].id_);
  EXPECT_EQ(5, (*neurons)[1].GetPosition()[0]);
  EXPECT_EQ(4, (*neurons)[1].GetPosition()[1]);
  EXPECT_EQ(3, (*neurons)[1].GetPosition()[2]);
  EXPECT_EQ(1.123, (*neurons)[0].GetDiameter());
}

TEST(SimulationObjectUtilTest, RemoveFromSimulation) {
  Simulation<> simulation(TEST_NAME);
  auto* rm = simulation.GetResourceManager();

  auto neurons = rm->Get<Neuron>();

  Neuron neuron;
  neurons->push_back(Neuron());
  EXPECT_EQ(1u, neurons->size());

  auto&& to_be_removed = (*neurons)[0];
  to_be_removed.RemoveFromSimulation();
  neurons->Commit();
  EXPECT_EQ(0u, neurons->size());
}

TEST(SimulationObjectUtilTest, Soa_IO) { RunSoaIOTest(); }

TEST(SimulationObjectUtilTest, ForEachDataMember) {
  std::vector<Neurite> neurites;
  Neuron neuron1(neurites, std::array<double, 3>{4, 5, 6});

  auto neurons = Neuron::NewEmptySoa();
  neurons.push_back(neuron1);

  uint16_t counter = 0;
  auto verify = [&](auto* data_member, const std::string& dm_name) {
    ASSERT_EQ(1u, data_member->size());
    counter++;
    if (dm_name != "neurites_" && dm_name != "position_" &&
        dm_name != "diameter_") {
      FAIL() << "Data member " << dm_name << "does not exist" << std::endl;
    }
  };

  neurons.ForEachDataMember(verify);
  EXPECT_EQ(3u, counter);
}

TEST(SimulationObjectUtilTest, ForEachDataMemberIn) {
  std::vector<Neurite> neurites;
  Neuron neuron1(neurites, std::array<double, 3>{4, 5, 6});

  auto neurons = Neuron::NewEmptySoa();
  neurons.push_back(neuron1);

  uint16_t counter = 0;
  auto verify = [&](auto* data_member, const std::string& dm_name) {
    ASSERT_EQ(1u, data_member->size());
    counter++;
    if (dm_name != "neurites_" && dm_name != "position_") {
      FAIL() << "Lambda must not be called for data member " << dm_name
             << std::endl;
    }
  };

  neurons.ForEachDataMemberIn(std::set<std::string>{"neurites_", "position_"},
                              verify);
  EXPECT_EQ(2u, counter);
}

struct VerifyPosition {
  void operator()(std::vector<std::array<double, 3>>* data_member,
                  const std::string& dm_name) {
    ASSERT_EQ(1u, data_member->size());
    if (dm_name == "position_") {
      EXPECT_EQ(4, (*data_member)[0][0]);
      EXPECT_EQ(5, (*data_member)[0][1]);
      EXPECT_EQ(6, (*data_member)[0][2]);
    } else {
      FAIL() << "Functor must not be called for data member " << dm_name
             << std::endl;
    }
  }

  void operator()(...) { FAIL() << "Should not be called" << std::endl; }
};

// for one data member check if the pointer contains the right data
TEST(SimulationObjectUtilTest, ForEachDataMemberInDetailed) {
  std::vector<Neurite> neurites;
  Neuron neuron1(neurites, std::array<double, 3>{4, 5, 6});

  auto neurons = Neuron::NewEmptySoa();
  neurons.push_back(neuron1);

  neurons.ForEachDataMemberIn(std::set<std::string>{"position_"},
                              VerifyPosition());
}

TEST(SimulationObjectUtilTestDeathTest, ForEachDataMemberInNonExistantDm) {
  auto neurons = Neuron::NewEmptySoa();
  auto lambda = [&]() {
    std::set<std::string> dm = {"position_", "does_not_exist"};
    neurons.ForEachDataMemberIn(dm, [](auto*, const std::string&) {});
  };

  ASSERT_DEATH(lambda(),
               ".*Please check your config file. The following data members do "
               "not exist: does_not_exist, .*");
}

TEST(SimulationObjectUtilTest, ToBackend) {
  EXPECT_EQ(typeid(Neuron), typeid(ToBackend<Neuron, Scalar>));
  EXPECT_EQ(typeid(SoaNeuron), typeid(ToBackend<Neuron, Soa>));
}

TEST(SimulationObjectUtilTest, ToScalar) {
  EXPECT_EQ(typeid(Neuron), typeid(ToScalar<Neuron>));
}

TEST(SimulationObjectUtilTest, ToSoa) {
  EXPECT_EQ(typeid(SoaNeuron), typeid(ToSoa<Neuron>));
}

BDM_SIM_OBJECT(TestThisMD, SimulationObject) {
  BDM_SIM_OBJECT_HEADER(TestThisMDExt, 0, foo_);

 public:
  TestThisMDExt() {}

  int AnotherFunction() { return 123; }

  int SomeFunction() { return ThisMD()->AnotherFunction(); }

  vec<int> foo_;
};

BDM_SIM_OBJECT(TestThisMDSubclass, TestThisMD) {
  BDM_SIM_OBJECT_HEADER(TestThisMDSubclassExt, 0, foo_);

 public:
  TestThisMDSubclassExt() {}

  int AnotherFunction() { return 321; }

  vec<int> foo_;
};

TEST(SimulationObjectUtilTest, ThisMD) {
  TestThisMDSubclass t;
  EXPECT_EQ(321, t.SomeFunction());
}

TEST(SimulationObjectUtilTest, GetSoPtr) {
  Simulation<> simulation(TEST_NAME);
  auto* rm = simulation.GetResourceManager();

  for (uint64_t i = 0; i < 10; i++) {
    rm->New<Neuron>();
  }
  rm->Get<Neuron>()->Commit();
  EXPECT_EQ(10u, rm->GetNumSimObjects());

  auto neurons = rm->Get<Neuron>();
  for (uint64_t i = 0; i < 10; i++) {
    EXPECT_EQ(i, (*neurons)[i].GetSoPtr().GetElementIdx());
  }
}

TEST(SimulationObjectUtilTest, IsSoType) {
  Neuron neuron;

  EXPECT_TRUE(neuron.template IsSoType<Neuron>());
  EXPECT_TRUE(neuron.template IsSoType<SoaNeuron>());
  EXPECT_FALSE(neuron.template IsSoType<Cell>());
  EXPECT_FALSE(neuron.template IsSoType<SoaCell>());

  EXPECT_TRUE(neuron.IsSoType(&neuron));
  SoaNeuron soaneuron;
  EXPECT_TRUE(neuron.IsSoType(&soaneuron));
  Cell cell;
  EXPECT_FALSE(neuron.IsSoType(&cell));
  SoaCell soacell;
  EXPECT_FALSE(neuron.IsSoType(&soacell));
}

TEST(SimulationObjectUtilTest, ReinterpretCast) {
  Cell cell;

  auto&& neuron_rc1 = cell.template ReinterpretCast<Neuron>();
  bool r1 = std::is_same<Neuron, std::decay_t<decltype(neuron_rc1)>>::value;
  EXPECT_TRUE(r1);

  auto&& neuron_rc2 = cell.template ReinterpretCast<SoaNeuron>();
  bool r2 = std::is_same<Neuron, std::decay_t<decltype(neuron_rc2)>>::value;
  EXPECT_TRUE(r2);

  Neuron neuron;
  SoaNeuron soa;

  auto&& neuron_rc3 = cell.template ReinterpretCast(&neuron);
  bool r3 = std::is_same<Neuron, std::decay_t<decltype(neuron_rc3)>>::value;
  EXPECT_TRUE(r3);

  auto&& neuron_rc4 = cell.template ReinterpretCast(&soa);
  bool r4 = std::is_same<Neuron, std::decay_t<decltype(neuron_rc4)>>::value;
  EXPECT_TRUE(r4);
}

// }  // namespace simulation_object_util_test_internal
}  // namespace bdm

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
